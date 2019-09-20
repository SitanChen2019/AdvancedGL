#version 330 core
layout (location = 0) in vec3 aPos;
layout( location = 1) in vec3 aNormal;

layout( location = 2) in ivec4 aBoneID1;
layout( location = 3) in ivec4 aBoneID2;
layout( location = 4) in ivec4 aBoneID3;
layout( location = 5) in ivec4 aBoneID4;

layout( location = 6) in vec4 aBoneWeight1;
layout( location = 7) in vec4 aBoneWeight2;
layout( location = 8) in vec4 aBoneWeight3;
layout( location = 9) in vec4 aBoneWeight4;


const int MAX_BONES = 200;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 boneMatrices[MAX_BONES];

out vec3 outNormal;
out vec3 outFragPos;


void main()
{
    mat4 boneTransform = boneMatrices[aBoneID1.x]* aBoneWeight1.x;
    boneTransform +=boneMatrices[aBoneID1.y] * aBoneWeight1.y;
    boneTransform +=boneMatrices[aBoneID1.z] * aBoneWeight1.z;
    boneTransform +=boneMatrices[aBoneID1.w] * aBoneWeight1.w;


    boneTransform += boneMatrices[aBoneID2.x] * aBoneWeight2.x;
    boneTransform +=boneMatrices[aBoneID2.y] * aBoneWeight2.y;
    boneTransform +=boneMatrices[aBoneID2.z] * aBoneWeight2.z;
    boneTransform +=boneMatrices[aBoneID2.w] * aBoneWeight2.w;

    boneTransform += boneMatrices[aBoneID3.x] * aBoneWeight3.x;
    boneTransform +=boneMatrices[aBoneID3.y] * aBoneWeight3.y;
    boneTransform +=boneMatrices[aBoneID3.z] * aBoneWeight3.z;
    boneTransform +=boneMatrices[aBoneID3.w] * aBoneWeight3.w;

    boneTransform += boneMatrices[aBoneID4.x] * aBoneWeight4.x;
    boneTransform +=boneMatrices[aBoneID4.y] * aBoneWeight4.y;
    boneTransform +=boneMatrices[aBoneID4.z] * aBoneWeight4.z;
    boneTransform +=boneMatrices[aBoneID4.w] * aBoneWeight4.w;

    gl_Position =  projection* view * model * boneTransform * vec4(aPos, 1.0f);
    outFragPos = (view * model * boneTransform *  vec4( aPos, 1.0f)).xyz;
    outNormal =  (transpose(inverse(view)) * model * boneTransform * vec4( aNormal, 0.0f)).xyz;
}

