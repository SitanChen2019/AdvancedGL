//
// Created by sitan on 19-9-18.
//
#include <iostream>
#include "animator.h"
#include <glm/gtx/string_cast.hpp>
#include "glad/glad.h"

Animator::Animator(AnimationData&& data)
:m_data( std::move(data) )
{
    m_boneFinalMatrix.resize( m_data.bones.size(), IdentifyMatrix4 );
}

Animator::Animator(const aiScene *pScene) {
    loadFromScene(pScene);
    m_boneFinalMatrix.resize( m_data.bones.size(), IdentifyMatrix4 );
}

void Animator::loadFromScene(const aiScene* pScene)
{
    int animationNum = pScene->mNumAnimations;
    m_data.animations.resize( animationNum );
    for( int i =0 ;i < animationNum; ++i )
    {
        Animation& animation = m_data.animations[i];
        const aiAnimation& ai_animation = *(pScene->mAnimations[i]);
        std::cout <<  ai_animation.mName.C_Str() << std::endl;
        animation.name = std::string(  ai_animation.mName.C_Str() );
        animation.duration = (float)ai_animation.mDuration;
        animation.ticksPerSceond = (float)ai_animation.mTicksPerSecond;

        std::cout << animation.name << " has " << ai_animation.mNumChannels << " channels." << std::endl;
        animation.channels.resize( ai_animation.mNumChannels);
        for( unsigned j = 0; j < ai_animation.mNumChannels; ++j )
        {
            NodeAnimation& nodeAnim = animation.channels[j];
            const aiNodeAnim& ai_nodeAnim = *(ai_animation.mChannels[j]);

            assert( ai_nodeAnim.mNumPositionKeys == ai_nodeAnim.mNumRotationKeys
                    && ai_nodeAnim.mNumRotationKeys == ai_nodeAnim.mNumScalingKeys );

            nodeAnim.nodeName = ai_nodeAnim.mNodeName.C_Str();

            nodeAnim.rotationKeys.resize( ai_nodeAnim.mNumRotationKeys );
            for(unsigned k = 0 ; k < ai_nodeAnim.mNumRotationKeys; ++ k )
            {
                nodeAnim.rotationKeys[k].first = (float)ai_nodeAnim.mRotationKeys[k].mTime;
                nodeAnim.rotationKeys[k].second.x = ai_nodeAnim.mRotationKeys[k].mValue.x;
                nodeAnim.rotationKeys[k].second.y = ai_nodeAnim.mRotationKeys[k].mValue.y;
                nodeAnim.rotationKeys[k].second.z = ai_nodeAnim.mRotationKeys[k].mValue.z;
                nodeAnim.rotationKeys[k].second.w = ai_nodeAnim.mRotationKeys[k].mValue.w;
            }

            nodeAnim.positionKeys.resize( ai_nodeAnim.mNumPositionKeys );
            for(unsigned k = 0; k < ai_nodeAnim.mNumPositionKeys; ++k )
            {
                nodeAnim.positionKeys[k].first = (float)ai_nodeAnim.mPositionKeys[k].mTime;
                nodeAnim.positionKeys[k].second.x = ai_nodeAnim.mPositionKeys[k].mValue.x;
                nodeAnim.positionKeys[k].second.y = ai_nodeAnim.mPositionKeys[k].mValue.y;
                nodeAnim.positionKeys[k].second.z = ai_nodeAnim.mPositionKeys[k].mValue.z;
            }

            nodeAnim.scaleKeys.resize( ai_nodeAnim.mNumScalingKeys );
            for(unsigned k = 0; k < ai_nodeAnim.mNumScalingKeys; ++k )
            {
                nodeAnim.scaleKeys[k].first = (float)ai_nodeAnim.mScalingKeys[k].mTime;
                nodeAnim.scaleKeys[k].second.x = ai_nodeAnim.mScalingKeys[k].mValue.x;
                nodeAnim.scaleKeys[k].second.y = ai_nodeAnim.mScalingKeys[k].mValue.y;
                nodeAnim.scaleKeys[k].second.z = ai_nodeAnim.mScalingKeys[k].mValue.z;
            }

        }
    }

    m_data.rootNode = new AnimationNode;
    loadAnimationNode(*m_data.rootNode, pScene->mRootNode, nullptr);

    int totalBoneNum = 0;
    for (unsigned i = 0; i < pScene->mNumMeshes; ++i) {
        totalBoneNum += pScene->mMeshes[i]->mNumBones;
    }


    m_data.bones.reserve(totalBoneNum);
    m_data.meshBoneDataList.resize( pScene->mNumMeshes );
    for (unsigned i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh *pMesh = pScene->mMeshes[i];
        BoneWeightsForMesh& boneWeightsList = m_data.meshBoneDataList[i];

        boneWeightsList.resize( pMesh->mNumVertices );
        for(unsigned j = 0; j < pMesh->mNumBones; ++j )
        {
            const aiBone&  ai_bone = *(pMesh->mBones[j]);
            std::string boneName  = ai_bone.mName.C_Str();

            int boneID;
            auto it = m_data.boneMap.find(boneName);
            if( it == m_data.boneMap.end() )
            {
                Bone bone;
                bone.boneName = boneName;
                for(int m = 0; m < 4; ++m )
                    for( int n =0; n < 4 ; ++n )
                    {
                        bone.offsetMatrix[n][m] = ai_bone.mOffsetMatrix[m][n];
                    }

                boneID = (int)m_data.bones.size();
                bone.id = boneID;
                m_data.boneMap.insert(std::make_pair(boneName, boneID));
                m_data.bones.push_back(bone);
            }
            else
            {
                boneID = it->second;
#ifdef CST_DEBUG
                Bone& bone  = m_data.bones[boneID];
                    bool match = true;
                    for(int m = 0; m < 4; ++m )
                        for( int n =0; n < 4 ; ++n )
                        {
                            if(  bone.offsetMatrix[m][n] != ai_bone.mOffsetMatrix[m][n] )
                            {
                                match = false;
                            }
                        }

                    if( !match)
                        std::cout << "Bone " << boneID << " dismatch" << std::endl;
#endif
            }

            for( unsigned k = 0; k < ai_bone.mNumWeights; ++k )
            {
                const aiVertexWeight& ai_vertexWeight = ai_bone.mWeights[k];
                boneWeightsList[ai_vertexWeight.mVertexId].push_back(BoneWeight{boneID,ai_vertexWeight.mWeight});
            }
        }

    }

#ifdef CST_DEBUG
    std::array<int,17> vertexBoneCountArray {0};
        for( auto& meshBoneDatas : m_data.meshBoneDataList)
        {
            for( auto& vertexBoneDatas : meshBoneDatas )
            {
                if( vertexBoneDatas.size() <  16 )
                    vertexBoneCountArray[ vertexBoneDatas.size() ] += 1;
                else
                    vertexBoneCountArray[16] += 1;
            }
        }

        for( int i = 0; i< 17; ++i )
        {
            std::cout << "Vertex with "<< i << " m_data.bones is " << vertexBoneCountArray[i] << std::endl;
        }
#endif //CST_DEBUG

}


void Animator::loadAnimationNode( AnimationNode& node,  aiNode* ai_node , AnimationNode* parentNode )
{
    node.pParent = parentNode;
    node.nodeName = ai_node->mName.C_Str();
    for(int i = 0; i < 4; ++i )
        for( int j =0; j < 4 ; ++j )
        {
            node.transform[j][i] = ai_node->mTransformation[i][j];
        }

    node.pChildren.resize( ai_node->mNumChildren );
    for( unsigned i = 0; i < ai_node->mNumChildren; ++i )
    {
        node.pChildren[i] = new AnimationNode;
        loadAnimationNode( *node.pChildren[i], ai_node->mChildren[i], &node );
    }
}



void Animator::update( float delta_time )
{

    if( m_currentAnimationId !=-1 )
    {
        m_animationTime +=  delta_time * m_speed;
        float mod_time = fmod(m_animationTime * m_data.animations[m_currentAnimationId].ticksPerSceond, m_data.animations[m_currentAnimationId].duration);
        AnimationNode* pNode = m_data.rootNode;
        updateAnimationNode( pNode, mod_time, IdentifyMatrix4);
    }

}


void Animator::updateAnimationNode( AnimationNode* pNode, float mod_time , const Matrix4& parentMatrix)
{
    const Animation& animation = m_data.animations[m_currentAnimationId];
    NodeAnimation* pNodeAnimation = findChannel( animation.channels, pNode->nodeName );
    Matrix4 selfMatrix = pNode->transform;

    if( pNodeAnimation )
    {
        //get current rotation
        Matrix4  rotateMat = getRotateMatrixByTime( pNodeAnimation, mod_time );

        //get current scale matrix
        Matrix4  scaleMatrix = getScaleMatrixByTime( pNodeAnimation, mod_time );

        //get current position matrix
        Matrix4  positionMatrix = getPositionMatrixByTime( pNodeAnimation, mod_time );

        selfMatrix =  positionMatrix * rotateMat * scaleMatrix;
    }



    Matrix4 currentMatrix = parentMatrix*selfMatrix;

    Bone* pBone =  findBone( pNode->nodeName );
    if( pBone )
    {
        m_boneFinalMatrix[ pBone->id ] = currentMatrix * pBone->offsetMatrix ;

    }

    for( auto& pChildNode : pNode->pChildren )
    {
        updateAnimationNode( pChildNode,mod_time, currentMatrix);
    }
}

void Animator::pickAnimation( int idx )
{
    if( idx >= (int)m_data.animations.size() )
        return;

    if( idx != m_currentAnimationId )
    {
        m_currentAnimationId = idx;
        m_animationTime = 0;
    }
}

Bone*  Animator::findBone( const std::string& boneName )
{
    auto it = m_data.boneMap.find( boneName );
    if( it != m_data.boneMap.end()  )
    {
        int boneID = it->second;
        assert( boneID >= 0 && boneID < (int)m_data.bones.size() );
        return &( m_data.bones.at(boneID) );
    }
    else
    {
        return nullptr;
    }
}

NodeAnimation* Animator::findChannel( const Vector<NodeAnimation>& channels , std::string name  )
{
    for( const auto& nodeAnimation : channels )
    {
        if( nodeAnimation.nodeName == name )
            return const_cast<NodeAnimation*>(&nodeAnimation);
    }


    return nullptr;
}

Matrix4  Animator::getRotateMatrixByTime( NodeAnimation* pNodeAnimationInfo, float mod_time)
{
    if( pNodeAnimationInfo->rotationKeys.empty() )
        return IdentifyMatrix4;

    if( pNodeAnimationInfo->rotationKeys.size() == 1 )
        return glm::toMat4( pNodeAnimationInfo->rotationKeys.back().second );

    int preID = 0;
    int curID = 1;

    for( ; curID < (int)pNodeAnimationInfo->rotationKeys.size() - 1; ++preID,++curID )
    {
        if( pNodeAnimationInfo->rotationKeys[curID].first > mod_time)
            break;
    }

    Quat curQuat = glm::mix( pNodeAnimationInfo->rotationKeys.at(preID).second,
              pNodeAnimationInfo->rotationKeys.at(curID).second,
              (mod_time - pNodeAnimationInfo->rotationKeys.at(preID).first)/
              (pNodeAnimationInfo->rotationKeys.at(curID).first - pNodeAnimationInfo->rotationKeys.at(preID).first) );

    curQuat = glm::normalize( curQuat );
    return glm::toMat4(curQuat);
}

Matrix4  Animator::getScaleMatrixByTime( NodeAnimation* pNodeAnimationInfo, float mod_time)
{
    if( pNodeAnimationInfo->scaleKeys.empty() )
        return IdentifyMatrix4;

    if( pNodeAnimationInfo->scaleKeys.size() == 1 )
        return glm::scale(IdentifyMatrix4,  pNodeAnimationInfo->scaleKeys.back().second );

    int preID = 0;
    int curID = 1;

    for( ; curID < (int)pNodeAnimationInfo->scaleKeys.size() - 1; ++preID,++curID )
    {
        if( pNodeAnimationInfo->scaleKeys[curID].first > mod_time)
            break;
    }

    Vec3 curScale = glm::mix( pNodeAnimationInfo->scaleKeys.at(preID).second,
                             pNodeAnimationInfo->scaleKeys.at(curID).second,
                             (mod_time - pNodeAnimationInfo->scaleKeys.at(preID).first)/
                             (pNodeAnimationInfo->scaleKeys.at(curID).first - pNodeAnimationInfo->scaleKeys.at(preID).first) );

    return glm::scale(IdentifyMatrix4,  curScale );
}

Matrix4  Animator::getPositionMatrixByTime( NodeAnimation* pNodeAnimationInfo, float mod_time)
{
    if( pNodeAnimationInfo->positionKeys.empty() )
        return IdentifyMatrix4;

    if( pNodeAnimationInfo->positionKeys.size() == 1 )
        return glm::translate(IdentifyMatrix4,  pNodeAnimationInfo->positionKeys.back().second );

    int preID = 0;
    int curID = 1;

    for( ; curID < (int)pNodeAnimationInfo->positionKeys.size() - 1; ++preID,++curID )
    {
        if( pNodeAnimationInfo->positionKeys[curID].first > mod_time)
            break;
    }

    Vec3 position = glm::mix( pNodeAnimationInfo->positionKeys.at(preID).second,
                              pNodeAnimationInfo->positionKeys.at(curID).second,
                              (mod_time - pNodeAnimationInfo->positionKeys.at(preID).first)/
                              (pNodeAnimationInfo->positionKeys.at(curID).first - pNodeAnimationInfo->positionKeys.at(preID).first) );

    return glm::translate(IdentifyMatrix4,  position );
}

//Vector<Mat4Sequence>  Animator::calcuateVertexBoneMatrices()
//{
//    Vector<Mat4Sequence> retList;
//    retList.resize( m_data.meshBoneDataList.size() );
//
//
//    for( int i = 0, meshCount = m_data.meshBoneDataList.size();i < meshCount; ++i  )
//    {
//        Mat4Sequence& matSeqForMesh = retList[i];
//        BoneWeightsForMesh& meshBoneData = m_data.meshBoneDataList[i];
//        int vertexCount = meshBoneData.size();
//        matSeqForMesh.resize(vertexCount, Matrix4{0});
//        for( int j =0 ; j< vertexCount; ++j )
//        {
//            auto& vertexBoneData = meshBoneData[j];
//            float totalWeight = 0;
//
//            for( int k = 0; k < vertexBoneData.size(); ++k )
//            {
//                auto a = m_boneFinalMatrix[ vertexBoneData[k].boneId]*Vec4(0,0,0,1);
//                assert(a.w == 1);
//
//                totalWeight += vertexBoneData[k].weight;
//                matSeqForMesh[j] += vertexBoneData[k].weight * m_boneFinalMatrix[ vertexBoneData[k].boneId];
//            }
//        }
//    }
//
//    return retList;
//}