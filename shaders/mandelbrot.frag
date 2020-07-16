#version 330 core

in vec2 v_uv;

uniform float u_gamma;

uniform mat3 u_uv_to_fs;

layout (origin_upper_left) in vec4 gl_FragCoord;

out vec4 o_color;

vec3 palette(int it, const int max_it)
{
	vec3 res;
	float a = 0.1f;
	float n = float(it);
	res.r = 0.5f * sin(a * n) + 0.5f;
	res.g = 0.5f * sin(a * n + 2.094f) + 0.5f;
	res.b = 0.5f * sin(a * n + 4.188f) + 0.5f;
	return res;
}

void main()
{
	vec2 uv = gl_FragCoord.xy;// - vec2(640.f, 360.f);
	vec3 fs = u_uv_to_fs * vec3(uv, 1.0f);
	float x0 = fs.x / fs.z;
	float y0 = fs.y / fs.z;
	float x = 0;
	float y = 0;
	int it = 0;
	const int max_it = 1000;
	while(x*x + y*y < 4 && it < max_it)
	{
		float tmp = x*x - y*y + x0;
		y = 2*x*y + y0;
		x = tmp;
		++it;
	}

	o_color = vec4(palette(it, max_it), 1.0);
}