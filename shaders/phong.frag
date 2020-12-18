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

uniform vec3 u_w_camera_position;

in Varying
{
	vec2 uv;
	vec3 w_position;
	vec3 w_normal;
	vec3 w_tangent;
} v_;

out vec4 o_color;

vec2 parallaxMapping(vec2 uv, vec3 tbn_to_view, float height)
{
	return uv;
	float scale = 0.1;
	vec2 p = tbn_to_view.xy / tbn_to_view.z * (height * scale);
	return uv - p;
}

void main()
{
	vec3 w_wo = normalize(u_w_camera_position - v_.w_position);
	vec3 w_normal = normalize(v_.w_normal);
	vec2 uv = v_.uv;
	if((u_tex_flags & 8) != 0)
	{
		vec3 w_tangent = normalize(v_.w_tangent);
		vec3 w_btg = cross(w_normal, w_tangent);
		mat3 tbn_to_world = mat3(w_tangent, w_btg, w_normal);
		mat3 world_to_tbn = transpose(tbn_to_world);
		vec4 texture_nh = texture2D(u_t_nh, v_.uv);
		vec3 tbn_to_view = world_to_tbn * w_wo;

		uv = parallaxMapping(v_.uv, tbn_to_view, texture_nh.a);
		if(uv.x > 1 || uv.y > 1 || uv.x < 0 || uv.y < 0)
			discard;

		vec3 texture_normal = texture2D(u_t_nh, uv).xyz;
		texture_normal = (texture_normal * 2.f - 1.f);
		w_normal = normalize(tbn_to_world * texture_normal);
	}
	
	vec3 w_rwo = reflect(-w_wo, w_normal);
	int i=0;

	vec3 u_diffuse = u_c_diffuse;
	if((u_tex_flags & 1) != 0)
		u_diffuse = u_diffuse * texture2D(u_t_diffuse, uv).xyz;

	
	vec4 u_glossy = u_c_glossy;

	vec3 u_emissive = u_c_emissive;

	vec3 res = u_emissive + u_diffuse * u_ambiant;
	while(i<MAX_LIGHTS)
	{
		Light light = u_lights[i];
		if(light.type == 0)		continue;

		vec3 incomming_radiance = vec3(0, 0, 0);
		vec3 to_light;
		if(light.type == 1) // point light
		{
			to_light = light.position - v_.w_position;
			float dist2 = dot(to_light, to_light);
			to_light = normalize(to_light);
			float cos_wi = dot(to_light, w_normal);
			incomming_radiance = light.Le / dist2;
		}
		else//(light.type == 2) // directional light
		{
			to_light = -light.direction;
			incomming_radiance = light.Le;
		}

		float diffuse_rho = max(0.f, dot(w_normal, to_light));
		vec3 diffuse = diffuse_rho * u_diffuse;
		float glossy_rho = diffuse_rho > 0 ? pow(max(0.f, dot(w_rwo, to_light)), u_glossy.a) : 0.f;
		vec3 glossy = glossy_rho * u_glossy.rgb;
		vec3 reflectance = diffuse + glossy;
		res += reflectance * incomming_radiance;
		++i;
	}
	//res = vec3(v_uv.x, 0, v_uv.y);
	o_color = vec4(res, 1.0f);
	
	vec3 dfx = dFdx(o_color.xyz);
	vec3 dfy = dFdy(o_color.xyz);

	//o_color = vec4((abs(dfx) + abs(dfy)) * 10, 1);
	return;
}