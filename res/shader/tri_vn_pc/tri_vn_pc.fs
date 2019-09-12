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
   vec3 nOutNormal = normalize( outNormal );

   float diffuse = max( dot(nOutNormal,-nLightDir),0 );

   vec3 reflectDir = reflect( nLightDir, nOutNormal);
   reflectDir = normalize(reflectDir);
   vec3 toEyeDir = vec3(0) - outFragPos;
   toEyeDir = normalize( toEyeDir);
   float spec = pow( max(0, dot( toEyeDir, reflectDir) ),128);
   spec = 0;


   FragColor.xyz =  sqrt( 0.2 + 0.8*(diffuse + spec) )*lightColor*diffMat;
   FragColor.a = 1;
}
