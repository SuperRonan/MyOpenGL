#version 330 core

layout (triangles) in;

layout (line_strip, max_vertices=2) out;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

in vec3 v_w_position[3];
in vec3 v_w_normal[3];

mat3 normalMatrix(in mat4 transform)
{
    mat3 t = mat3(transform);
    return transpose(inverse(t));
}

void main()
{
    float magnitude = 0.25;

    vec3 center = (v_w_position[0] + v_w_position[1] + v_w_position[2]) / 3.f;
    vec3 normal = normalize((v_w_normal[0] + v_w_normal[1] + v_w_normal[2]) / 3.f);

    mat4 VP = u_P * u_V;

    vec3 base = center;
    vec3 vector = normal;

    vec3 point1 = base;
    vec3 point2 = base + vector * magnitude;

    gl_Position = VP * vec4(point1, 1.f);
    EmitVertex();

    gl_Position = VP * vec4(point2, 1.f);
    EmitVertex();
    EndPrimitive();
}