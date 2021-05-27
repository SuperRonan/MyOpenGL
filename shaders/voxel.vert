#version 460 core

out ivec3 g_grid_id;

uniform ivec3 grid_dims;

void main()
{
	g_grid_id = ivec3(
		gl_VertexID / (grid_dims.z * grid_dims.y),
		(gl_VertexID / (grid_dims.z)) % grid_dims.y,
		gl_VertexID % (grid_dims.z)
	);

}