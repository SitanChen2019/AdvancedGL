#version 400

layout( location = 0 ) in vec3 aPosition;
uniform mat4 view;
uniform mat4 projection;

void main()
{
     gl_Position = view*projection*vec4( aPosition, 1.0f);
}