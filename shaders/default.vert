#version 460 core

layout(location = 0) in vec3 a_grid_id;

out vec2 v_uv;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;


void main()
{
	const mat4 MVP = u_P * u_V * u_M;
	vec3 v_pos = vec3(MVP * vec4(a_grid_id, 1));
	gl_Position = vec4(v_pos, 1);
	v_uv = vec2(1, 1);
}