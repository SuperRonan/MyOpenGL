#version 330 core
// In model space
layout(location = 0) in vec3 a_position;
// In model space
layout(location = 1) in vec3 a_normal;
// In model space
layout(location = 2) in vec2 a_uv;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec2 v_uv;
out vec3 v_m_normal;

void main()
{
	mat4 MVP = u_P * u_V * u_M;
	vec4 h_pos = vec4(a_position, 1.0);
	gl_Position = MVP * h_pos;
	
	v_uv = a_uv;
	v_m_normal = a_normal;
}