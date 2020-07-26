#version 330 core
uniform vec3 u_diffuse;

uniform vec4 u_glossy;


out vec4 o_color;

void main()
{
	vec3 res = vec3(0.f, 0.f, 0.f);

	res += u_diffuse;

	o_color = vec4(res, 1.0f);
}