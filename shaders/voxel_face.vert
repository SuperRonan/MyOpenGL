#version 460 core

// Last one is the face id
out ivec4 g_grid_id;

uniform ivec3 grid_dims;

void main()
{
	g_grid_id = ivec4(
		gl_VertexID / (grid_dims.z * grid_dims.y * 3),
		(gl_VertexID / (grid_dims.z * 3)) % grid_dims.y,
		(gl_VertexID / 3) % (grid_dims.z),
		(gl_VertexID % 3)
	);

}