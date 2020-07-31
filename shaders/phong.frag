#version 330 core
uniform vec3 u_diffuse;

uniform vec4 u_glossy;

uniform vec3 u_emissive;

#define MAX_LIGHTS 5

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

in vec2 v_uv;

out vec4 o_color;

void main()
{
	vec3 res = u_emissive + u_diffuse * u_ambiant;
	vec3 w_normal = normalize(v_w_normal);
	vec3 w_wo = normalize(u_w_camera_position - v_w_position);
	vec3 w_rwo = reflect(-w_wo, w_normal);
	int i=0;
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