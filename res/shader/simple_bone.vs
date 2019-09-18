#version 330 core
 layout (location = 0) in vec4 aPos;
 layout (location = 1) in mat4 boneMatrix;

uniform mat4 view;
uniform mat4 projection;


 void main()
 {
     gl_Position = projection * view * aPos;

 }