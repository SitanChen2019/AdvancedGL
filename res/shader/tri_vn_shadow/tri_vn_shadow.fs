#version 330 core


in vec3 outNormal;
in vec3 outFragPos;

out vec4 FragColor;

uniform sampler2D shadowTexture;

uniform mat4 view;
uniform mat4 lightViewProj;

uniform vec3 diffMat = vec3(0.5, 0.5, 0.5);
uniform vec3 lightColor =  vec3(1);
uniform vec3 lightDir = vec3(-1,-1,-1);


float getShadow( vec3 fragPos )
{
    vec4 tmp = lightViewProj*vec4(fragPos,1);
    tmp = tmp/tmp.w;
    tmp = tmp*0.5 + 0.5;

    float depth = texture( shadowTexture, tmp.xy).r;
    return depth < tmp.z ? 1 : 0;
}

void main()
{
   float shadow = getShadow( outFragPos );

   vec3 nLightDir = normalize( (view*vec4(lightDir,0)).xyz );
   vec3 nOutNormal = normalize( outNormal );

   float diffuse = max( dot(nOutNormal,-nLightDir),0 );

   vec3 reflectDir = reflect( nLightDir, nOutNormal);
   reflectDir = normalize(reflectDir);
   vec3 toEyeDir = vec3(0) - outFragPos;
   toEyeDir = normalize( toEyeDir);
   float spec = pow( max(0, dot( toEyeDir, reflectDir) ),128);
   spec = 0;


   FragColor.xyz =  sqrt( 0.2 + (1-shadow)*0.8*(diffuse + spec) )*lightColor*diffMat;
   FragColor.a = 1;
}
