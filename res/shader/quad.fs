#version 330 core

in vec2 ourTexCoord;
out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
   vec3 color = texture(ourTexture,ourTexCoord).rgb;
   FragColor = vec4(color,1);
}