#version 460 core

out vec4 res;

in vec2 v_uv;

void main()
{
	res = vec4(v_uv, 0, 1);
}