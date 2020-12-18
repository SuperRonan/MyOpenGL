#version 330 core
// In model space
layout(location = 0) in vec3 a_position;
// In model space
layout(location = 1) in vec3 a_normal;
// In model space
layout(location = 2) in vec3 a_tangent;
// In model space
layout(location = 3) in vec2 a_uv;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out Varying
{
	vec2 uv;
	vec3 c_position;
	vec3 c_normal;
	vec3 c_tangent;
} v_;

mat3 normalMatrix(in mat4 transform)
{
    mat3 t = mat3(transform);
    return transpose(inverse(t));
}

void extractSRT(mat4 M, out vec3 s, out vec3 r, out vec3 t)
{
	t = M[3].xyz;
	// 3x3 rotation block
	mat3 R;
	for(int i=0; i<3; ++i)
	{
		s[i] = length(M[i].xyz);
		R[i] = M[i].xyz / s[i];
	}
	// Extract the Euler rotation angles
	r.x = atan(R[1][2], R[2][2]);
	r.y = atan(-R[0][2], length(vec2(R[1][2], R[2][2])));
	r.z = atan(R[0][1], R[0][0]);
}

mat4 transform(vec3 s, vec3 r, vec3 t)
{
	mat3 S3 = mat3(s.x, 0, 0, 0, s.y, 0, 0, 0, s.z);
	mat3 Rx = mat3(cos(r.x), sin(r.x), 0, -sin(r.x), cos(r.x), 0, 0, 0, 1);
	mat3 Ry = mat3(cos(r.y), 0, -sin(r.y), 0, 1, 0, sin(r.y), 0, cos(r.y));
	mat3 Rz = mat3(1, 0, 0, 0, cos(r.z), sin(r.z), 0, -sin(r.z), cos(r.z));
	mat3 R3 = Rx * Ry * Rz;
	
	mat4 S = mat4(S3);
	mat4 R = mat4(R3);
	mat4 T = mat4(1);
	T[3].xyz = t;

	mat4 M = T * R * S;
	return M;
}


mat4 cartoonMV(mat4 M, mat4 V)
{
	mat4 MV_f = V * M;
	vec3 s, r, t;
	extractSRT(MV_f, s, r, t);

	float stats = 1;
	//r = floor(r * stats) / stats;

	MV_f = transform(s, r, t);
	return MV_f;
}

void main()
{
	mat4 MV = cartoonMV(u_M, u_V);
	mat3 MV_normal_mat = normalMatrix(MV);
	mat4 MVP = u_P * MV;
	vec4 h_pos = vec4(a_position, 1.0f);
	gl_Position = MVP * h_pos;
	

	v_.uv = a_uv;
	v_.c_position = (MV * h_pos).xyz;
	v_.c_normal = normalize(MV_normal_mat * a_normal);
	v_.c_tangent = normalize(MV_normal_mat * a_tangent);
}