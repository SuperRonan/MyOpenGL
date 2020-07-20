#version 410 core

//uniform dmat3 u_uv_to_fs;

uniform uvec2 u_scale;
uniform uvec2 u_tx;
uniform uvec2 u_ty;

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
	dmat3 u_uv_to_fs = dmat3(1.0);
	u_uv_to_fs[0][0] = packDouble2x32(u_scale);
	u_uv_to_fs[1][1] = packDouble2x32(u_scale);
	u_uv_to_fs[2][0] = packDouble2x32(u_tx);
	u_uv_to_fs[2][1] = packDouble2x32(u_ty);
	dvec2 uv = gl_FragCoord.xy;
	dvec3 fs = u_uv_to_fs * dvec3(uv, 1.0);
	double x0 = fs.x / fs.z;
	double y0 = fs.y / fs.z;
	double x = 0;
	double y = 0;
	int it = 0;
	const int max_it = 500;
	while(x*x + y*y < 4 && it < max_it)
	{
		double tmp = x*x - y*y + x0;
		y = 2*x*y + y0;
		x = tmp;
		++it;
	}

	o_color = vec4(palette(it, max_it), 1.0f);
}