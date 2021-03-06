//
// Created by Sitan Chen on 2019/9/3.
//

#include "ModelDisplay.h"
#include "global.h"
#include "model_loader.h"
#include "aabb_render.h"
#include "utilities.h"
#include "ModelDisplayRenderables.h"
#include "shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ShadowRender.h"

bool ModelDisplay::init() {

    m_pShadowRender = new ShadowRender();
    m_pShadowRender->initialize();

    ModelLoader loader(Global::SystemResPath("/models/skirt_01/nvqun2.obj").c_str());
    Vector<MeshData> tessellationDatas = loader.loadModel();

    MeshData floor = Shape::createXZRectangle(3,3);
    Shape::Translataion(floor, glm::translate( glm::identity<Matrix4>(), Vec3(0,-1,0)) );
    tessellationDatas.push_back( floor );

    for( auto& meshdata : tessellationDatas )
    {
        if(meshdata.normals.empty() )
        {
            Utilities::calculateSmoothNormal( meshdata.vertices, meshdata.indices, meshdata.normals );
        }

        MeshBuffer buffers;
        buffers.initBufferFromMeshData( meshdata );

        m_bbox.merge(meshdata.vertices);

        m_pShadowRender->addMeshBuffer(buffers);

        IRenderable* pRenderable1 = new SimpleRenderable(buffers);
        Global::renderWindow().addRenderable( pRenderable1 );
        m_wf_meshes.insert( pRenderable1 );

        IRenderable* pRenderable2 = new TriangleRenderable(buffers);
        Global::renderWindow().addRenderable( pRenderable2 );
        m_triagnle_meshes.insert( pRenderable2 );

        IRenderable* pRenderable3 = (IRenderable*)new TriangleWithVertexNormalRenderable(buffers);
        Global::renderWindow().addRenderable( pRenderable3 );
        m_smooth_triangle_meshes.insert( pRenderable3 );

        TriangleWithVertexNormalShadowRenderable* pRenderable4 = new TriangleWithVertexNormalShadowRenderable(buffers);
        pRenderable4->setShadowTexture( m_pShadowRender->getDepthTexture() );
        Global::renderWindow().addRenderable( pRenderable4 );
        m_shadow_triangle_meshes.insert( pRenderable4 );
    }

    AABBRender* pBoxrender = new AABBRender(m_bbox);
    m_otherMesh.insert( pBoxrender);
    Global::renderWindow().addRenderable( pBoxrender );


    Global::cameraControl().fitBox( m_bbox);

    showMeshGroup( m_otherMesh, false);
    displayAsShadowSmoothLighting();

    m_pUI = new ModelDisplayUIView(this);
    Global::uiManager().addUIView( m_pUI);

    DepthTextureRender* pDepthRenderable = new DepthTextureRender();
    Global::renderWindow().addRenderable( pDepthRenderable );
    m_otherMesh.insert( pDepthRenderable );
    pDepthRenderable->setColorTexture( m_pShadowRender->getDepthTexture() );
    pDepthRenderable->setEnabel(false);



    return true;
}


void ModelDisplay::displayAsPureColor()
{
    showMeshGroup(m_wf_meshes, true );
    showMeshGroup(m_triagnle_meshes, false );
    showMeshGroup(m_smooth_triangle_meshes, false );
    showMeshGroup(m_shadow_triangle_meshes, false);
}

void ModelDisplay::displayAsHardLighting()
{
    showMeshGroup(m_wf_meshes, false );
    showMeshGroup(m_triagnle_meshes, true );
    showMeshGroup(m_smooth_triangle_meshes, false );
    showMeshGroup(m_shadow_triangle_meshes, false);
}

void ModelDisplay::displayAsSmoothLighting()
{
    showMeshGroup(m_wf_meshes, false );
    showMeshGroup(m_triagnle_meshes, false );
    showMeshGroup(m_smooth_triangle_meshes, true );
    showMeshGroup(m_shadow_triangle_meshes, false);
}


void ModelDisplay::displayAsShadowSmoothLighting(){
    showMeshGroup(m_wf_meshes, false );
    showMeshGroup(m_triagnle_meshes, false );
    showMeshGroup(m_smooth_triangle_meshes, false );
    showMeshGroup(m_shadow_triangle_meshes, true);
}




void ModelDisplay::reloadShader() {
    Global::shaderMgr().reloadShader("tri_vn_pc");
    Global::shaderMgr().reloadShader("triangle");
}

bool ModelDisplay::update()
{
    renderShadowTexture();
    return true;
}

void ModelDisplay::renderShadowTexture()
{
    Vec3 lightDir = glm::vec3(-1,-1,-1);
    lightDir = glm::normalize(lightDir);

    float diagonalLength = m_bbox.diagonalLength();
    float r = diagonalLength/2;
    auto center =m_bbox.center();
    auto lightPos = m_bbox.center() - 2.f*r*lightDir ;
    Matrix4  viewMat = glm::lookAt( lightPos, center, Vec3(0,1,0) );
    Matrix4  projMat = glm::ortho(-r,r,-r,r,r,3.f*r);
    m_pShadowRender->renderDepthTexture( viewMat, projMat);

    Matrix4 lightViewProjmat = projMat*viewMat;
    for( auto pRendeable : m_shadow_triangle_meshes )
    {
        TriangleWithVertexNormalShadowRenderable* pShadowRenderable = dynamic_cast<TriangleWithVertexNormalShadowRenderable*>( pRendeable );
        if( pShadowRenderable )
            pShadowRenderable->setLightViewProjMat( lightViewProjmat );
    }
}

bool ModelDisplay::destroy()
{
    removeMeshGroup(m_shadow_triangle_meshes );
    removeMeshGroup(m_wf_meshes);
    removeMeshGroup(m_triagnle_meshes);
    removeMeshGroup(m_smooth_triangle_meshes);
    removeMeshGroup(m_otherMesh);

    Global::uiManager().removeUIView( m_pUI);
    delete m_pUI;
    m_pUI = nullptr;

    m_pShadowRender->destroy();
    delete m_pShadowRender;
    m_pShadowRender = nullptr;

    return true;
}

void ModelDisplay::removeMeshGroup(std::set<IRenderable*>& meshGroup)
{
    for( auto pRenderable : meshGroup )
    {
        Global::renderWindow().removeRenderable( pRenderable );
        delete pRenderable;
    }

    meshGroup.clear();

}

void ModelDisplay::showMeshGroup(const std::set<IRenderable*>& meshGroup, bool enable)
{
    for( auto pRenderable : meshGroup )
        pRenderable->setEnabel( enable );

}
