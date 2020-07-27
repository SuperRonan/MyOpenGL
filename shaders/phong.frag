#version 330 core
uniform vec3 u_diffuse;

uniform vec4 u_glossy;

#define MAX_LIGHTS 5

struct Light
{
	int type;
	// in world space
	vec3 position;
	vec3 Le;
};

uniform Light u_lights[MAX_LIGHTS];

in vec3 v_w_position;
in vec3 v_w_normal;

out vec4 o_color;

void main()
{
	vec3 res = vec3(0.f, 0.f, 0.f);
	vec3 w_normal = normalize(v_w_normal);
	int i=0;
	while(i<MAX_LIGHTS)
	{
		Light light = u_lights[i];
		if(light.type == 0)		continue;
		else if(light.type == 1) // point light
		{
			vec3 to_light = light.position - v_w_position;
			float dist2 = dot(to_light, to_light);
			to_light = normalize(to_light);
			float cos_wi = dot(to_light, w_normal);
			vec3 diff = max(0.f, cos_wi) * u_diffuse * light.Le / dist2;
			res += diff;
		}
		++i;
	}
	o_color = vec4(res, 1.0f);
}