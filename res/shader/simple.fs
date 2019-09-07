#version 330 core
uniform vec3 color;

out vec4 FragColor;
void main()
{
   FragColor = vec4(1,1,color.z,1);
}