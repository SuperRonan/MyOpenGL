#include <voxels/World.h>
#include <numbers>
#include <algorithm>
#include <lib/Transforms.h>
#include <lib/ShaderDesc.h>
#include <lib/ProgramDesc.h>
#include <lib/Material.h>

namespace perlin
{
	/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
	float interpolate(float a0, float a1, float w) {
		 //// You may want clamping by inserting:
		 // if (0.0 > w) return a0;
		 // if (1.0 < w) return a1;
		 //
		//return (a1 - a0) * w + a0;
		 // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
		//return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
		
		// Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
		return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
		 
	}

	/* Create random direction vector
	 */
	glm::vec2 randomGradient(int ix, int iy) {
		// Random float. No precomputed gradients mean this works for any number of grid coordinates
		float random = 2920.f * sin(ix * 21942.f + iy * 171324.f + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
		return glm::vec2{cos(random), sin(random) };
	}

	// Computes the dot product of the distance and gradient vectors.
	float dotGridGradient(int ix, int iy, float x, float y) {
		// Get gradient from integer coordinates
		glm::vec2 gradient = randomGradient(ix, iy);

		// Compute the distance vector
		float dx = x - (float)ix;
		float dy = y - (float)iy;

		// Compute the dot-product
		return (dx * gradient.x + dy * gradient.y);
	}

	// Compute Perlin noise at coordinates x, y
	float perlin(float x, float y) {
		// Determine grid cell coordinates
		int x0 = (int)x;
		int x1 = x0 + 1;
		int y0 = (int)y;
		int y1 = y0 + 1;

		// Determine interpolation weights
		// Could also use higher order polynomial/s-curve here
		float sx = x - (float)x0;
		float sy = y - (float)y0;

		// Interpolate between grid point gradients
		float n0, n1, ix0, ix1, value;

		n0 = dotGridGradient(x0, y0, x, y);
		n1 = dotGridGradient(x1, y0, x, y);
		ix0 = interpolate(n0, n1, sx);

		n0 = dotGridGradient(x0, y1, x, y);
		n1 = dotGridGradient(x1, y1, x, y);
		ix1 = interpolate(n0, n1, sx);

		value = interpolate(ix0, ix1, sy);
		return value;
	}
}


template <class T, int N>
T distanceTchebychev(lib::Vector<N, T> const& a, lib::Vector<N, T> const& b)
{
	lib::Vector<N, T> d = b - a;
	T res = std::abs(d[0]);
	for (int i = 1; i < N; ++i)
	{
		res = std::max(res, std::abs(d[i]));
	}
	return res;
}


_NODISCARD size_t std::hash<glm::ivec2>::operator()(glm::ivec2 const& cid) const noexcept
{
	return size_t(cid.x) << 32 + size_t(cid.y);
}


World::GridRayMarcher::GridRayMarcher(glm::vec2 const& source, glm::vec2 const& dir) :
	_vertical(std::abs(dir.x) > std::cos(std::numbers::pi / 4.0)),
	_pos(source),
	_dir_normalized(dir)
{
	if (_vertical)
	{
		_dir = _dir_normalized / std::abs(_dir_normalized.x);
	}
	else
	{
		_dir = _dir_normalized / std::abs(_dir_normalized.y);
	}
}

World::GridRayMarcher::GridRayMarcher(glm::vec2 const& source, glm::vec2 const& dir, bool vertical) :
	_vertical(vertical),
	_pos(source),
	_dir_normalized(dir)
{
	if (_vertical)
	{
		_dir = _dir_normalized / std::abs(_dir_normalized.x);
	}
	else
	{
		_dir = _dir_normalized / std::abs(_dir_normalized.y);
	}

	tan_alpha = _vertical ? (_dir_normalized.y / _dir_normalized.x) : (_dir_normalized.x / _dir_normalized.y);
}

void World::GridRayMarcher::firstStep()
{
	if (_vertical)
	{
		bool right = _dir_normalized.x > 0;
		float target = right ? std::ceil(_pos.x) : std::floor(_pos.x);
		float dx = target - _pos.x;
		float dy = dx * tan_alpha;

		_pos += glm::vec2(dx, dy);
		_pos.y = std::round(_pos.y);
	}
	else
	{
		bool up = _dir_normalized.y > 0;
		float target = up ? std::ceil(_pos.y) : std::floor(_pos.y);
		float dy = target - _pos.y;
		float dx = dy * tan_alpha;

		_pos += glm::vec2(dx, dy);
		_pos.x = std::round(_pos.x);
	}
}

void World::GridRayMarcher::stepOne()
{
	_pos += _dir;
}

glm::ivec2 World::GridRayMarcher::getId()const
{
	glm::ivec2 res;
	if (_vertical)
	{
		res = { _pos.x + 0.5, _pos.y };
		bool right = _dir_normalized.x > 0;
		if (!right) --res.x;
	}
	else
	{
		res = { _pos.x, _pos.y + 0.5 };
		bool up = _dir_normalized.y > 0;
		if (!up)	--res.y;
	}
	return res;
}

World::World(glm::ivec3 chunk_size) :
	_chunk_size(chunk_size)
{
	_load_radius = 6;
	_draw_distance = 6;


	_ids_buffer = std::vector<glm::ivec3>(_chunk_size.x * _chunk_size.y * _chunk_size.z);
	for (size_t x = 0; x < _chunk_size.x; ++x)
		for (size_t y = 0; y < _chunk_size.y; ++y)
			for (size_t z = 0; z < _chunk_size.z; ++z)
			{
				size_t aid = x * _chunk_size.y * _chunk_size.z + y * _chunk_size.z + z;
				_ids_buffer[aid] = { x, y, z };
			}

	glGenVertexArrays(1, &a_ids_vao);
	//glGenBuffers(1, &a_ids_vbo);

	//glBindVertexArray(a_ids_vao);

	//glBindBuffer(GL_ARRAY_BUFFER, a_ids_vbo);
	//glBufferData(GL_ARRAY_BUFFER, _ids_buffer.size() * sizeof(glm::ivec3), (void*)_ids_buffer.data(), GL_STATIC_DRAW);

	//glVertexAttribIPointer(0, 3, GL_INT, sizeof(glm::ivec3), (void*)0);
	//glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	_vox_prog = std::make_shared<lib::ProgramDesc>(lib::Material::shaderPath().string() + "voxel", true);
	_vox_prog->link();

}

World::ColumnInfo World::generateColumnInfo(glm::ivec2 cid)const
{
	ColumnInfo res;
	bool use_perlin = true;
	if (use_perlin)
	{
		float scale = 0.05;
		float p = perlin::perlin((cid.x + 0.5) * scale, (cid.y + 0.5) * scale);
		float h = p + 128;
		h = std::clamp(h, 0.0f, float(_chunk_size.y));
		res.dirt_start = h;
		res.dirt_height = 6;
	}
	else
	{
		const float twoPi = std::numbers::pi * 2;
		int ground_level = 16;
		glm::vec2 cpos = { cid.x + 0.5, cid.y + 0.5 };
		const float freq = 1.0 / 64.0;
		res.dirt_start = ground_level + (cos(cpos.x * freq * twoPi) + 1 + sin(cpos.y * freq * twoPi) + 1) * 4;
		res.dirt_height = 6;
	}
	return res;
}

void World::fillChunk(glm::ivec2 cid)
{
	Chunk& c = _chunks[cid];
	std::memset(&c[0], c.byteSize(), 0);
	for (int i = 0; i < _chunk_size.x; ++i)
	{
		for (int j = 0; j < _chunk_size.z; ++j)
		{
			glm::ivec2 c_column = { i, j };
			glm::ivec2 w_column = cid * glm::ivec2(_chunk_size.x, _chunk_size.z) + c_column;
			ColumnInfo clm = generateColumnInfo(w_column);
			glm::ivec3 idx = { i, 0, j };

			for (int k = 0; k < clm.dirt_start; ++k)
			{
				idx.y = k;
				c(idx) = Voxel{ 1 };
			}

			for (int k = clm.dirt_start; k < clm.dirt_start + clm.dirt_height; ++k)
			{
				idx.y = k;
				c(idx) = Voxel{ 2 };
			}

			idx.y = clm.dirt_start + clm.dirt_height;
			c(idx) = Voxel{ 3 };
		}
	}
}

glm::ivec2 World::getChunkId(glm::vec3 wpos)const
{
	glm::ivec2 res = {
		(wpos.x) / _chunk_size.x, (wpos.z) / _chunk_size.z
	};
	if (wpos.x < 0)
		--res.x;
	if (wpos.z < 0)
		--res.y;
	return res;
}

void World::update(glm::vec3 cam_pos)
{
	glm::ivec2 cam_chunk_id = getChunkId(cam_pos);

	for (int i = -_load_radius; i <= _load_radius; ++i)
	{
		for (int j = -_load_radius; j <= _load_radius; ++j)
		{
			glm::ivec2 cid = cam_chunk_id + glm::ivec2{ i, j };
			if (_chunks.find(cid) == _chunks.end())
			{
				std::cout << "Adding a new Chunk " << cid << std::endl;
				_chunks.insert(std::pair<glm::ivec2, Chunk>(cid, Chunk(_chunk_size)));
				fillChunk(cid);
				Chunk& chunk = _chunks[cid];
				chunk.createSSBO();
				chunk.updateSSBO();
			}
		}
	}
}

bool World::chunkIsVisible(lib::Camera<float> const& cam, ChunkToDraw const& chunk)const
{
	return true;
}

void World::buildDrawList(lib::Camera<float> const& cam)
{
	_draw_list.resize(0);
	
	const glm::ivec2 cam_gid = getChunkId(cam.getPosition());

	bool raster = false;

	if (raster)
	{
		const glm::vec2 cam2d_pos = { cam.getPosition().x, cam.getPosition().z };
		const glm::vec3 cam_dir_3d = cam.getDirection();
		const float fov = glm::radians(cam.fov());
		
		const glm::vec2 center_dir = glm::normalize(glm::vec2(cam_dir_3d.x, cam_dir_3d.z));
		const glm::vec2 left_dir = lib::rotate2D(fov / 2.0f) * center_dir;
		const glm::vec2 right_dir = lib::rotate2D(-fov / 2.0f) * center_dir;
		
		const glm::vec2 chunk_size = { _chunk_size.x, _chunk_size.z };

		GridRayMarcher center_grm(cam2d_pos / chunk_size, center_dir);
		GridRayMarcher left_grm(cam2d_pos / chunk_size, left_dir, center_grm._vertical);
		GridRayMarcher right_grm(cam2d_pos / chunk_size, right_dir, center_grm._vertical);

		left_grm.firstStep();
		right_grm.firstStep();

		const auto addDrawLine = [&](const glm::ivec2& left, const glm::ivec2& right, bool vertical)
		{
			const glm::ivec2 diff = right - left;
			int v = vertical ? 1 : 0;
			std::cout << diff[1 - v] << std::endl;
			int N = abs(diff[v]) + 1;
			std::cout << N << std::endl;
			int B = std::min(left[v], right[v]);
			for (int i = B; i < B + N; ++i)
			{
				glm::ivec2 cid;
				cid[1 - v] = left[1 - v];
				cid[v] = i;
				if (_chunks.contains(cid))
				{
					ChunkToDraw c;
					c.chunk = &_chunks[cid];
					c.id = cid;
					_draw_list.push_back(c);
				}
			}
		};

		{
			addDrawLine(left_grm.getId(), right_grm.getId(), center_grm._vertical);
		}

		while (false)
		{
			center_grm.stepOne();
			glm::ivec2 draw_id = center_grm.getId();
			if (_chunks.contains(draw_id))
			{
				ChunkToDraw c;
				c.chunk = &_chunks[draw_id];
				c.id = draw_id;
				_draw_list.push_back(c);
			}
			break;
		}
	}
	else
	{
		for (auto& [k, chunk] : _chunks)
		{
			// TODO only keep chunks that intersect the frustum
			if (distanceTchebychev(k, cam_gid) <= _draw_distance)
			{
				ChunkToDraw td;
				td.chunk = &chunk;
				td.id = k;

				glm::vec2 chunk_center = { (k.x + 0.5) * _chunk_size.x, (k.y + 0.5) * _chunk_size.z };
				glm::vec2 cam_pos = { cam.getPosition().x, cam.getPosition().z };

				td.distance = glm::distance(cam_pos, chunk_center);

				_draw_list.push_back(td);
			}
		}

		std::sort(_draw_list.begin(), _draw_list.end(), [](ChunkToDraw const& a, ChunkToDraw const& b)
			{
				return a.distance < b.distance;
			}
		);
	}
}

void World::draw(lib::Camera<float> const& cam)
{
	glBindVertexArray(a_ids_vao);

	glm::ivec2 cam_cid = getChunkId(cam.getPosition());
	glm::mat4 V = cam.getMatrixV();
	glm::vec3 recenter_v = { cam_cid.x * _chunk_size.x, 0, cam_cid.y * _chunk_size.z};
	glm::mat4 recenter_m = lib::translateMatrix<4, float>(-recenter_v);
	V = V * recenter_m;
	std::cout << V[3] << std::endl;
	glm::vec3 recentered_cam_pos = cam.getPosition() - recenter_v;

	_vox_prog->use();
	_vox_prog->setUniform("u_V", V);
	_vox_prog->setUniform("u_P", cam.getMatrixP());
	_vox_prog->setUniform("u_cam_pos", recentered_cam_pos);

	for(ChunkToDraw & td : _draw_list)
	{
		const glm::ivec2 recentered_cid = ( td.id);
		glm::vec3 chunk_base = { recentered_cid.x * _chunk_size.x, 0, recentered_cid.y * _chunk_size.z };
		glm::mat4 M = lib::translateMatrix<4, float>(chunk_base);

		_vox_prog->setUniform("u_M", M);
		_vox_prog->setUniform("grid_strides", td.chunk->strides());
		_vox_prog->setUniform("grid_dims", td.chunk->dims());

		td.chunk->bind();

		glDrawArrays(GL_POINTS, 0, td.chunk->size());
	}

	lib::ProgramDesc::useNone();
	glBindVertexArray(0);

}