#version 460 core

out vec4 res;

in vec2 v_uv;
in flat vec3 v_w_normal;
in flat int v_tex_id;

layout(location=0) uniform sampler2DArray t_diffuse;

void main()
{

//	if(v_uv.x < 0.6 && v_uv.x > 0.4 && v_uv.y < 0.6 && v_uv.y > 0.4)
//	{
//		vec3 v_color = abs(v_w_normal) * 0.75 + 0.25 * v_w_normal;
//		res = vec4(v_color, 1);
//	}
//	else
	{
		vec3 dif = texture(t_diffuse, vec3(v_uv, v_tex_id)).xyz;
		res = vec4(dif, 1);
		//res = vec4(v_uv, 0, 1);
	}
//	else
//	{
//		res = vec4(dif, 1);
//	}
//	else if(v_w_normal.x != 0)
//	{
//		res = vec4(0, v_uv.x, v_uv.y, 1);
//	}
//	else if(v_w_normal.y !=0 )
//	{
//		res = vec4(v_uv.x, 0, v_uv.y, 1);
//	}
//	else 
//	{
//		res = vec4(v_uv.x, v_uv.y, 0, 1);
//	}

	
}