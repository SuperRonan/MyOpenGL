#version 330 core

layout (triangles) in;

layout (line_strip, max_vertices=8) out;

uniform sampler2D u_t_normal;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

in vec3 v_w_position[3];
in vec3 v_w_normal[3];
in vec3 v_w_tangent[3];
in vec2 v_uv[3];

mat3 normalMatrix(in mat4 transform)
{
    mat3 t = mat3(transform);
    return transpose(inverse(t));
}

out vec3 v_color;

void emitLine(mat4 VP, vec3 base, vec3 vector, float magnitude, vec3 base_color, vec3 end_color)
{
    vec3 point1 = base;
    vec3 point2 = base + vector * magnitude;

    gl_Position = VP * vec4(point1, 1.f);
    v_color = base_color;
    EmitVertex();

    gl_Position = VP * vec4(point2, 1.f);
    v_color = end_color;
    EmitVertex();

}

void emitLine(mat4 VP, vec3 base, vec3 vector, float magnitude, vec3 color)
{
    emitLine(VP, base, vector, magnitude, color, color);
}

void main()
{
    float magnitude = 0.2;
    // in world space
    vec3 center = (v_w_position[0] + v_w_position[1] + v_w_position[2]) / 3.f;
    vec3 normal = normalize((v_w_normal[0] + v_w_normal[1] + v_w_normal[2]) / 3.f);
    vec3 tangent = normalize((v_w_tangent[0] + v_w_tangent[1] + v_w_tangent[2]) / 3.f);
    vec2 uv = (v_uv[0] + v_uv[1] + v_uv[2]) / 3.f;
    vec3 btg = cross(normal, tangent);

    mat3 TBN = (mat3(tangent, btg, normal));
    vec3 texture_normal = texture2D(u_t_normal, uv).xyz;
    //texture_normal = vec3(0.5, 0.5, 1);
    texture_normal = normalize(texture_normal * 2.f - vec3(1, 1, 1));
    //texture_normal = vec3(0, 0, 1);
    vec3 w_texture_normal = normalize(TBN * texture_normal);
    

    mat4 VP = u_P * u_V;

    vec3 base = center + normal * 0.01f;

    emitLine(VP, base, tangent, magnitude, vec3(1, 0, 0));
    emitLine(VP, base, btg, magnitude, vec3(0, 1, 0));
    emitLine(VP, base, normal, magnitude, vec3(0, 0, 1));
    
    emitLine(VP, base, w_texture_normal, magnitude, vec3(1, 1, 1));

    EndPrimitive();
}