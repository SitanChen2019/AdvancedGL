//
// Created by sitan on 19-9-17.
//

#include "CharacterAnimation.h"
#include "model_loader.h"
#include "animator.h"
#include "mesh_buffer.h"
#include "AnimationRenderable.h"
#include "global.h"


 bool CharacterAnimation::init()
 {
     //ModelLoader loader("../res/models/test/walk.dae");
     ModelLoader loader("../res/models/dancer_01/08dance.dae");


     const aiScene* pScene = loader.getScene();

     if( pScene->mNumAnimations == 0 )
        return false;


     Vector<BoneWeightsForMesh> meshBoneWeightsList;


     m_pAnimator = new Animator( pScene );
     m_pAnimator->pickAnimation(0);
     m_pAnimator->update(0);

     meshBoneWeightsList = m_pAnimator->getMeshesBoneWeights();

     Vector<MeshData> tessellationDatas = loader.loadModel();
     for( int i =0; i<tessellationDatas.size(); ++i)
     {
         MeshData& meshdata =  tessellationDatas[i];
         MeshBuffer buffers;
         buffers.initBufferFromMeshData( meshdata );
         AnimationRenderable* pRenderable = new AnimationRenderable(buffers, meshdata.vertices, meshBoneWeightsList[i]);
         pRenderable->updateBoneMatrix( m_pAnimator->getBoneMatrices() );
         Global::renderWindow().addRenderable( pRenderable );
         m_wf_meshes.push_back( pRenderable );

         //use the first frame vertex to fix the bounding box
         m_bbox.merge(  pRenderable->calcuateAnimationVertexList() );
     }

     Global::cameraControl().fitBox( m_bbox);

     m_pAnimator->setSpeed(0.4f);

     return true;
 }

 bool CharacterAnimation::update()
 {

    static double lastTime = 0;
    double now = glfwGetTime();
    if( lastTime == 0)
        lastTime = now;

    if( m_pAnimator )
        m_pAnimator->update( now - lastTime );

    lastTime = now;


     for( int i= 0, n = m_wf_meshes.size(); i<n ; ++i )
     {
         AnimationRenderable* pAnimationRenderable = dynamic_cast<AnimationRenderable*>(m_wf_meshes[i]);
         pAnimationRenderable->updateBoneMatrix( m_pAnimator->getBoneMatrices() );
     }


     return true;
 }

 bool CharacterAnimation::destroy()
 {
     removeMeshGroup(m_wf_meshes);

    if( m_pAnimator )
    {
        delete m_pAnimator;
        m_pAnimator = nullptr;
    }
    return true;
 }


void CharacterAnimation::removeMeshGroup(std::vector<IRenderable*>& meshGroup)
{
    for( auto pRenderable : meshGroup )
    {
        Global::renderWindow().removeRenderable( pRenderable );
        delete pRenderable;
    }

    meshGroup.clear();

}

void CharacterAnimation::showMeshGroup(const std::vector<IRenderable*>& meshGroup, bool enable)
{
    for( auto pRenderable : meshGroup )
        pRenderable->setEnabel( enable );

}