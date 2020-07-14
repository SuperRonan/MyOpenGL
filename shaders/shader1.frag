#version 330 core

uniform vec3 u_color;

in vec2 v_uv;
in vec3 v_m_normal;

out vec4 o_color;

void main()
{
	o_color.xy = v_uv;
	o_color.z = 0.2;
	o_color.w = 1.0;
	vec3 m_normal = normalize(v_m_normal);
	o_color = vec4(abs(m_normal), 1.0);
}