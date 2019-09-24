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



float getShadow1( vec3 fragPos )
{
    vec2 poissonDisk[4] = vec2[](
      vec2( -0.94201624, -0.39906216 ),
      vec2( 0.94558609, -0.76890725 ),
      vec2( -0.094184101, -0.92938870 ),
      vec2( 0.34495938, 0.29387760 )
    );

    vec4 tmp = lightViewProj*vec4(fragPos,1);
    tmp = tmp/tmp.w;
    tmp = tmp*0.5 + 0.5;

    vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);

    float depth;
    float bias = 0.005;
    int  shadowTestTrueCount= 0;

    for( int i= 0; i<4; ++i)
    {
        depth = texture( shadowTexture, tmp.xy + texelSize*poissonDisk[i]).r;
        if( tmp.z- bias > depth)
        {
           shadowTestTrueCount+= 1;
        }
    }

    return  0.25*shadowTestTrueCount;
}


//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float getShadow( vec3 fragPos )
{
    vec2 poissonDisk[16] = vec2[](
       vec2( -0.94201624, -0.39906216 ),
       vec2( 0.94558609, -0.76890725 ),
       vec2( -0.094184101, -0.92938870 ),
       vec2( 0.34495938, 0.29387760 ),
       vec2( -0.91588581, 0.45771432 ),
       vec2( -0.81544232, -0.87912464 ),
       vec2( -0.38277543, 0.27676845 ),
       vec2( 0.97484398, 0.75648379 ),
       vec2( 0.44323325, -0.97511554 ),
       vec2( 0.53742981, -0.47373420 ),
       vec2( -0.26496911, -0.41893023 ),
       vec2( 0.79197514, 0.19090188 ),
       vec2( -0.24188840, 0.99706507 ),
       vec2( -0.81409955, 0.91437590 ),
       vec2( 0.19984126, 0.78641367 ),
       vec2( 0.14383161, -0.14100790 )
    );

    vec4 tmp = lightViewProj*vec4(fragPos,1);
    tmp = tmp/tmp.w;
    tmp = tmp*0.5 + 0.5;

    vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);

    float depth;
    float bias = 0.005;
    int  shadowTestTrueCount= 0;

    int N = 16;
    float inverse_n = 1.0/N;
    for( int i= 0; i<N; ++i)
    {
        int index = i;//int(16.0*random(floor(fragPos.xyz*1000.0), i))%16;
        depth = texture( shadowTexture, tmp.xy + texelSize*poissonDisk[index]).r;
        if( tmp.z- bias > depth)
        {
           shadowTestTrueCount+= 1;
        }
    }

    return  inverse_n*shadowTestTrueCount;
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
