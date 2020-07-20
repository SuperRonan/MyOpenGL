#version 330 core

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

vec2 complex_prod(vec2 a, vec2 b)
{
	return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

int escape(vec2 z0, const int max_it)
{
	vec2 z = vec2(0.0f, 0.0f);
	int it=0;
	while(dot(z, z) < 4.0f && it<max_it)
	{
		z = complex_prod(z, z) + z0;
		++it;
	}
	return it;
}

void main()
{
	vec2 uv = gl_FragCoord.xy;
	vec3 fs = u_uv_to_fs * vec3(uv, 1.0f);
	vec2 z0 = fs.xy / fs.z;
	
	const int max_it = 500;

	int it = escape(z0, max_it);

	o_color = vec4(palette(it, max_it), 1.0);
}