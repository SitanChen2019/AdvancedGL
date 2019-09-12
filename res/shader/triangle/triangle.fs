#version 330 core


in vec3 outNormal;
in vec3 outFragPos;

out vec4 FragColor;

uniform mat4 view;


uniform vec3 diffMat = vec3(0.5, 0.5, 0.5);
uniform vec3 lightColor =  vec3(1);
uniform vec3 lightDir = vec3(-1,-1,-1);

void main()
{
   vec3 nLightDir = normalize( lightDir );

   float diffuse = max( dot(outNormal,-nLightDir),0 );

   vec3 reflectDir = reflect( nLightDir, outNormal);
   reflectDir = normalize(reflectDir);
   vec3 toEyeDir = vec3(0) - outFragPos;
   toEyeDir = normalize( toEyeDir);
   float spec = pow( max(0, dot( toEyeDir, reflectDir) ),64);


   FragColor.xyz = vec3(diffuse + spec);
   FragColor.a = 1;
}
