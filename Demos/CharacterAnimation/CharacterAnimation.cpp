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
     ModelLoader loader("../res/models/test/walk.dae");
     //ModelLoader loader("../res/models/dancer_01/08dance.dae");



     const aiScene* pScene = loader.getScene();
     if( pScene->mNumAnimations > 0 )
     {
         m_pAnimator = new Animator( pScene );
     }

     Vector<MeshData> tessellationDatas = loader.loadModel();
     for( auto& meshdata : tessellationDatas )
     {
         MeshBuffer buffers;
         buffers.initBufferFromMeshData( meshdata );

         m_bbox.merge(meshdata.vertices);

         IRenderable* pRenderable1 = new AnimationRenderable(buffers, meshdata.vertices);
         Global::renderWindow().addRenderable( pRenderable1 );
         m_wf_meshes.push_back( pRenderable1 );
     }


     Global::cameraControl().fitBox( m_bbox);

     m_pAnimator->pickAnimation(0);

     return true;
 }

 bool CharacterAnimation::update()
 {

    static double lastTime = 0;
    double now = glfwGetTime();
    if( lastTime == 0)
        lastTime = now;

    if( m_pAnimator )
        m_pAnimator->update(now-lastTime);

    lastTime = now;

     Vector<Mat4Sequence> matList = m_pAnimator->calcuateVertexBoneMatrices();

     AABB box;
     for( int i= 0, n = m_wf_meshes.size(); i<n ; ++i )
     {
         AnimationRenderable* pAnimationRenderable = dynamic_cast<AnimationRenderable*>(m_wf_meshes[i]);
         pAnimationRenderable->updateBoneMatrix( matList[i], box);
     }

     Global::cameraControl().fitBox( box );

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