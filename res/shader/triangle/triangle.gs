#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


uniform mat4 projection;

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
   return normalize(cross(a, b));
}

out vec3 outNormal;
out vec3 outFragPos;

void main() {
    outNormal = GetNormal();
    outFragPos = gl_in[0].gl_Position.xyz;
    gl_Position = projection*gl_in[0].gl_Position;
    EmitVertex();
    outFragPos = gl_in[1].gl_Position.xyz;
    gl_Position = projection*gl_in[1].gl_Position;
    EmitVertex();
    outFragPos = gl_in[2].gl_Position.xyz;
    gl_Position = projection*gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}