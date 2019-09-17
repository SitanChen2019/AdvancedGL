#version 330 core


in vec3 outNormal;
in vec3 outFragPos;
in vec3 worldFragPos;

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

    vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);
    float depth;

    depth = texture( shadowTexture, tmp.xy).r;

    float bias = 0.005;
    return tmp.z- bias <= depth ? 0 : 1;
}

void main()
{


   vec3 nLightDir = normalize( (view*vec4(lightDir,0)).xyz );
   vec3 nOutNormal = normalize( outNormal );

   float diffuse = max( dot(nOutNormal,-nLightDir),0 );

   vec3 reflectDir = reflect( nLightDir, nOutNormal);
   reflectDir = normalize(reflectDir);
   vec3 toEyeDir = vec3(0) - outFragPos;
   toEyeDir = normalize( toEyeDir);
   float spec = pow( max(0, dot( toEyeDir, reflectDir) ),128);
   spec = 0;

   //FragColor.xyz = vec3(1 - getShadow( worldFragPos)) ;
   float shadow = getShadow( worldFragPos);
   FragColor.xyz =  sqrt( 0.2 + (1-shadow)*0.8*(diffuse + spec) )*lightColor*diffMat;
   FragColor.a = 1;
}
