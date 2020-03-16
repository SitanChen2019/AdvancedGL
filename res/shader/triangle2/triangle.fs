#version 330 core



out vec4 FragColor;

uniform mat4 view;


uniform vec3 diffMat = vec3(0.5, 0.5, 0.5);


void main()
{
   FragColor.xyz =  diffMat;
   FragColor.a = 1;
}
