#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <voxels/Chunk.h>

#include <lib/ProgramDesc.h>
#include <lib/Transforms.h>
#include <lib/StreamOperators.h>
#include <lib/Camera.h>
#include <lib/ProgramDesc.h>

#include <compare>

#include <iostream>

template <>
struct std::hash<glm::ivec2>
{
	_NODISCARD size_t operator()(glm::ivec2 const& cid) const noexcept;
};

class World
{
public:

	class GridRayMarcher
	{
	public:

		// 0 -> intersect horizontal lines
		// 1 -> intersect vertical lines
		bool _vertical;

		glm::vec2 _pos;
		glm::vec2 _dir_normalized, _dir;

		float tan_alpha;

	public:

		GridRayMarcher(glm::vec2 const& source, glm::vec2 const& dir);

		GridRayMarcher(glm::vec2 const& source, glm::vec2 const& dir, bool vertical);

		void firstStep();

		void stepOne();

		glm::ivec2 getId()const;
	};

protected:

	std::unordered_map<glm::ivec2, Chunk> _chunks;

	glm::ivec3 _chunk_size;

	std::vector<glm::ivec3> _ids_buffer;
	GLuint a_ids_vao, a_ids_vbo;

	struct ChunkToDraw
	{
		Chunk* chunk;
		glm::ivec2 id;
		float distance;
	};

	std::vector<ChunkToDraw> _draw_list;

	int _load_radius, _draw_distance;

	std::shared_ptr<lib::ProgramDesc> _vox_prog;

public:

	World(glm::ivec3 chunk_size = { 32, 256, 32 });

	struct ColumnInfo
	{
		int dirt_start, dirt_height;
	};

	ColumnInfo generateColumnInfo(glm::ivec2 cid)const;

	void fillChunk(glm::ivec2 cid);

	glm::ivec2 getChunkId(glm::vec3 wpos)const;

	void update(glm::vec3 cam_pos);

	void draw(lib::Camera<float> const& cam);

	bool chunkIsVisible(lib::Camera<float> const& cam, ChunkToDraw const& chunk)const;

	void buildDrawList(lib::Camera<float> const& cam);

};