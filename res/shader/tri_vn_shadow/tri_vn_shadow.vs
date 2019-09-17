#version 330 core
layout (location = 0) in vec3 aPos;
layout( location = 1) in vec3 aNormal;

uniform mat4 view;
uniform mat4 projection;

out vec3 outNormal;
out vec3 outFragPos;

void main()
{
    gl_Position =  projection* view * vec4(aPos, 1.0f);

    outFragPos = (view * vec4( aPos, 1.0f)).xyz;
    outNormal =  (transpose(inverse(view)) * vec4( aNormal, 0.0f)).xyz;
}