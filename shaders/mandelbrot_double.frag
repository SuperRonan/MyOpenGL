#version 410 core

uniform dmat3 u_uv_to_fs;

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

dvec2 complex_prod(dvec2 a, dvec2 b)
{
	return dvec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

int escape(dvec2 z0, const int max_it)
{
	dvec2 z = dvec2(0.0, 0.0);
	int it=0;
	while(dot(z, z) < 4.0 && it<max_it)
	{
		z = complex_prod(z, z) + z0;
		++it;
	}
	return it;
}

void main()
{
	dvec2 uv = gl_FragCoord.xy;
	dvec3 fs = u_uv_to_fs * dvec3(uv, 1.0);
	dvec2 z0 = fs.xy / fs.z;
	
	const int max_it = 500;

	int it = escape(z0, max_it);

	o_color = vec4(palette(it, max_it), 1.0f);
}