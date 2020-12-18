#version 330 core

// 0 (1) diffuse
// 1 (2) glossy
// 2 (4) emissive
// 3 (8) normal
uniform int u_tex_flags;

uniform sampler2D u_t_diffuse;

uniform vec3 u_c_diffuse;

uniform vec4 u_c_glossy;

uniform vec3 u_c_emissive;

uniform sampler2D u_t_nh;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 5
#endif

struct Light
{
	int type;
	// in world space
	vec3 position;
	vec3 direction;
	vec3 Le;
};

uniform vec3 u_ambiant;

uniform Light u_lights[MAX_LIGHTS];

in Varying
{
	vec2 uv;
	vec3 c_position;
	vec3 c_normal;
	vec3 c_tangent;
} v_;

out vec4 o_color;


void main()
{
	vec3 c_wo = normalize(v_.c_position);
	vec3 c_normal = normalize(v_.c_normal);
	vec2 uv = v_.uv;
	if((u_tex_flags & 8) != 0)
	{
		vec3 c_tangent = normalize(v_.c_tangent);
		vec3 c_btg = cross(c_normal, c_tangent);
		mat3 tbn_to_camera = mat3(c_tangent, c_btg, c_normal);
		mat3 camera_to_tbn = transpose(tbn_to_camera);
		vec4 texture_nh = texture2D(u_t_nh, v_.uv);
		vec3 tbn_to_view = camera_to_tbn * c_wo;

		uv = v_.uv;
//		if(uv.x > 1 || uv.y > 1 || uv.x < 0 || uv.y < 0)
//			discard;

		vec3 tbn_normal = texture2D(u_t_nh, uv).xyz;
		tbn_normal = (tbn_normal * 2.f - 1.f);
		c_normal = normalize(tbn_to_camera * tbn_normal);
	}
	
	vec3 c_rwo = reflect(-c_wo, c_normal);
	int i=0;

	vec3 u_diffuse = u_c_diffuse;
	if((u_tex_flags & 1) != 0)
		u_diffuse = u_diffuse * texture2D(u_t_diffuse, uv).xyz;

	
	vec4 u_glossy = u_c_glossy;

	vec3 u_emissive = u_c_emissive;

	vec3 res = u_emissive + u_diffuse * u_ambiant;
//
//	mat4 MV = cartoonMV(u_M, u_V);
//	mat3 MV_normal_mat = normalMatrix(MV);
//
//	while(i<MAX_LIGHTS)
//	{
//		Light light = u_lights[i];
//		if(light.type == 0)		continue;
//
//		vec3 incomming_radiance = vec3(0, 0, 0);
//		vec3 c_to_light;
//		if(light.type == 1) // point light
//		{
//			vec3 c_light_pos = (MV * vec4(light.position, 1)).xyz;
//			c_to_light = c_light_pos - v_.c_position;
//			float dist2 = dot(to_light, to_light);
//			c_to_light = normalize(to_light);
//			float cos_wi = dot(c_to_light, c_normal);
//			incomming_radiance = light.Le / dist2;
//		}
//		else//(light.type == 2) // directional light
//		{
//			c_to_light = - (MV_normal_mat * vec4(light.direction, 0)).xyz;
//			incomming_radiance = light.Le;
//		}
//
//		float diffuse_rho = max(0.f, dot(c_normal, c_to_light));
//		vec3 diffuse = diffuse_rho * u_diffuse;
//		float glossy_rho = diffuse_rho > 0 ? pow(max(0.f, dot(c_rwo, c_to_light)), u_glossy.a) : 0.f;
//		vec3 glossy = glossy_rho * u_glossy.rgb;
//		vec3 reflectance = diffuse + glossy;
//		res += reflectance * incomming_radiance;
//		++i;
//	}

	o_color = vec4(res, 1.0f);
}