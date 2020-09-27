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

uniform sampler2D u_t_normal;

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

in vec3 v_w_position;
in vec3 v_w_normal;
in vec3 v_w_tangent;

in vec2 v_uv;

out vec4 o_color;

void main()
{
	vec3 w_normal = normalize(v_w_normal);
	if((u_tex_flags & 8) != 0)
	{
		vec3 w_tangent = normalize(v_w_tangent);
		vec3 w_btg = cross(w_normal, w_tangent);
		mat3 TBN = (mat3(w_tangent, w_btg, w_normal));
		vec3 texture_normal = texture2D(u_t_normal, v_uv).xyz;
		texture_normal = (texture_normal * 2.f - 1.f);
		w_normal = normalize(TBN * texture_normal);
	}
	
	vec3 w_wo = normalize(u_w_camera_position - v_w_position);
	vec3 w_rwo = reflect(-w_wo, w_normal);
	int i=0;

	vec3 u_diffuse = u_c_diffuse;
	if((u_tex_flags & 1) != 0)
		u_diffuse = u_diffuse * texture2D(u_t_diffuse, v_uv).xyz;

	
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
			to_light = light.position - v_w_position;
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
}