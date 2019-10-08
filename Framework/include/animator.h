//
// Created by sitan on 19-9-18.
//

#ifndef ADVANCEDGL_ANIMATOR_H
#define ADVANCEDGL_ANIMATOR_H

#include <map>
#include "render_type.h"


#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>

struct BoneWeight
{
    BoneWeight()
    {
        boneId = -1;
        weight = 0;
    }

    BoneWeight( int id, float w)
    {
        boneId = id;
        weight = w;
    }

    int boneId;
    float weight;
};

using BoneWeightsForVertex = Vector<BoneWeight>;  //for single vertex
using BoneWeightsForMesh = Vector<BoneWeightsForVertex>; //for single mesh

struct Bone
{
    std::string boneName;
    int         id;
    Matrix4     offsetMatrix; // transform from model local space to bone space
};

struct AnimationNode
{
    std::string nodeName;
    Matrix4     transform;   //transform from node space to the space of its parent
    AnimationNode* pParent;
    Vector<AnimationNode*> pChildren;

    ~AnimationNode()
    {
        for( auto* pChild : pChildren )
        {
            delete pChild;
        }
        pChildren.clear();
    }
};

struct NodeAnimation
{
    std::string nodeName;
    Vector<std::pair<float,Vec3>> positionKeys;
    Vector<std::pair<float,Vec3>> scaleKeys;
    Vector<std::pair<float,Quat>> rotationKeys;
};

struct Animation
{
    std::string name;
    float duration;
    float ticksPerSceond;
    Vector<NodeAnimation> channels;
};


struct AnimationData
{
    AnimationData() = default;

    ~AnimationData()
    {
        if(rootNode)
            delete rootNode;
    }

    AnimationData(const AnimationData& ) = delete;
    AnimationData& operator=( const AnimationData& ) = delete;

    AnimationData(AnimationData&& other)
    {
        animations.swap( other.animations );
        std::swap(rootNode, other.rootNode);
        bones.swap( other.bones);
        boneMap.swap(other.boneMap);
        meshBoneDataList.swap( other.meshBoneDataList );
    }

    Vector<Animation> animations;
    AnimationNode* rootNode = nullptr;
    Vector<Bone> bones;
    std::map<std::string, int> boneMap;
    Vector<BoneWeightsForMesh> meshBoneDataList;
};


class Animator
{
public:
    Animator( const aiScene* pScene);

    Animator(AnimationData&& data);

    void update( float delta_time );
    void pickAnimation( int idx );
    void setSpeed( float speed )
    {
        m_speed = speed;
    }

    //Vector<Mat4Sequence> calcuateVertexBoneMatrices();

    const Vector<BoneWeightsForMesh>& getMeshesBoneWeights()
    {
        return m_data.meshBoneDataList;
    }

    const Mat4Sequence& getBoneMatrices()
    {
        return m_boneFinalMatrix;
    }



private:
    void updateAnimationNode( AnimationNode* pNode, float mod_time , const Matrix4& parentMatrix);

    Matrix4  getRotateMatrixByTime( NodeAnimation* pNodeAnimationInfo, float time);
    Matrix4  getScaleMatrixByTime( NodeAnimation* pNodeAnimationInfo, float time);
    Matrix4  getPositionMatrixByTime( NodeAnimation* pNodeAnimationInfo, float time);

    Bone*    findBone( const std::string& boneName );
    NodeAnimation* findChannel( const Vector<NodeAnimation>& channels , std::string chanelName );


    void loadFromScene(const aiScene* pScene);

    void loadAnimationNode( AnimationNode& node,  aiNode* ai_node , AnimationNode* parentNode );

private:
    int m_currentAnimationId = -1;
    float m_animationTime = 0;
    AnimationData m_data;

    //output
    Mat4Sequence m_boneFinalMatrix;

    float m_speed = 1.0f;
};

#endif //ADVANCEDGL_ANIMATOR_H
