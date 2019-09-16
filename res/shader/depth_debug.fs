#version 330 core

in vec2 ourTexCoord;
out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
   float depth = texture(ourTexture,ourTexCoord).r;
   FragColor = vec4(depth,depth,depth,1);
}