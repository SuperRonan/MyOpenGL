#version 460 core

layout(points) in;

layout(triangle_strip, max_vertices=12) out;


uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

in ivec3 g_grid_id[1];

out vec3 v_w_pos;
out flat vec3 v_w_normal;
out vec2 v_uv;
out flat int v_tex_id;

uniform ivec3 grid_strides;
uniform ivec3 grid_dims;

uniform vec3 u_cam_pos;

int gridId(ivec3 ids)
{
	return  ids.x * grid_strides.x + 
			ids.y * grid_strides.y + 
			ids.z * grid_strides.z;
}


struct Voxel
{
	int id;
};

#define N_TYPES 256

struct Property
{
	int flags;
	int face_ids[6];
	int _pad;
};

layout(packed, binding=10) uniform Props
{
	Property properties[N_TYPES];
};

restrict readonly layout(std430, binding=0) buffer sanple_Ttext
{
	Voxel voxels[];
};

vec3 axisFromId(int id)
{
	vec3 res = vec3(0);
	res[id] = 1;
	return res;
}

bool inGrid(ivec3 id)
{
	bvec3 res = greaterThanEqual(id, ivec3(0)) && lessThan(id, grid_dims);
	return all(res);
}

bool frontIsOpaque(ivec3 front_gid)
{	
	bool res = false;
	if(inGrid(front_gid))
	{
		Voxel front_vx = voxels[gridId(front_gid)];
		if(front_vx.id != 0)
		{
			Property p = properties[front_vx.id];
			res = bool(p.flags & 1);
		}
	}
	return res;
}

void swap(inout int a, inout int b)
{
	int c = a;
	a = b;
	b = c;
}

// axis : 0 -> x / 1 -> y / 2 -> z
// s (sign) : 0 -> + / 1 -> -
void emitFace(int axis, int s, vec3 box_center, float half_size, mat4 MVP, int tex_id)
{
	const float fs = (s == 1) ? -1 : +1;
	const int u_axis_id = (axis + 1) % 3;
	const int v_axis_id = (axis + 2) % 3;

	int u_tex_axis_id = u_axis_id;
	int v_tex_axis_id = v_tex_id;
	
	if(axis == 0) swap(u_tex_axis_id, v_tex_axis_id);
	
	const vec3 n = fs * axisFromId(axis); const vec3 hn = n * 0.5;
	const vec3 u = fs * axisFromId(u_axis_id); const vec3 hu = u * 0.5;
	const vec3 v = fs * axisFromId(v_axis_id); const vec3 hv = v * 0.5;

	if(axis == 0)
		v_uv = vec2(1, 1-s); // 1, 1
	else
		v_uv = vec2(0, 1-s);
	v_w_pos = box_center + hn + -1 * hu + -1 * hv;
	v_w_normal = n;
	v_tex_id = tex_id;
	gl_Position = MVP * vec4(v_w_pos, 1);
	EmitVertex();

	if(axis == 0)
		v_uv = vec2(1-s, 0); // 1, 0
	else
		v_uv = vec2(1-s, 1);
	v_w_pos = box_center + hn + fs * hu + -fs * hv;
	v_w_normal = n;
	v_tex_id = tex_id;
	gl_Position = MVP * vec4(v_w_pos, 1);
	EmitVertex();

	if(axis == 0)
		v_uv = vec2(s, 1); // 0, 1
	else
		v_uv = vec2(s, 0);
	v_w_pos = box_center + hn + -fs * hu + fs * hv;
	v_w_normal = n;
	v_tex_id = tex_id;
	gl_Position = MVP * vec4(v_w_pos, 1);
	EmitVertex();

	if(axis == 0)
		v_uv = vec2(0, s); // 0, 0
	else
		v_uv = vec2(1, s);
	v_w_pos = box_center + hn + 1 * hu + 1 * hv;
	v_w_normal = n;
	v_tex_id = tex_id;
	gl_Position = MVP * vec4(v_w_pos, 1);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	const float voxel_size = 0.5;

	const ivec3 grid_id = g_grid_id[0];
	
	const int gid = gridId(grid_id);
	const Voxel v = voxels[gid];

	if(v.id != 0) // 0 => empty
	{
		Property vxp = properties[v.id];

		const vec3 grid_pos = vec3(grid_id) + 0.5;
		const mat4 MVP = u_P * u_V * u_M;
		//const vec3 c_grid_pos = (u_V * u_M * vec4(grid_pos, 1)).xyz;
		const vec3 c_grid_pos = (u_M * vec4(grid_pos, 1)).xyz - u_cam_pos;

		for(int axis=0; axis<3; ++axis)
		{
			int s = c_grid_pos[axis] < 0 ? 0 : 1;
			{
				// Manual culling ?
				ivec3 grid_offset = ivec3(0);
				grid_offset[axis] = (s == 0) ? 1 : -1;
				ivec3 front_id = grid_id + grid_offset;
				bool hidden = frontIsOpaque(front_id);
				if(!hidden)
				{
					const int face_id = axis * 2 + s;
					const int face_tex_id = vxp.face_ids[face_id];
					emitFace(axis, s, grid_pos, voxel_size, MVP, face_tex_id);
				}
			}
		}
	}	
}