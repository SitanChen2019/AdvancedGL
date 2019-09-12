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

bool ModelDisplay::init() {

    Vector<MeshData> tessellationDatas = ModelLoader::loadModel("../res/models/skirt_01/nvqun2.obj");

    for( auto& meshdata : tessellationDatas )
    {
        if(meshdata.normals.empty() )
        {
            Utilities::calculateSmoothNormal( meshdata.vertices, meshdata.indices, meshdata.normals );
        }

        MeshBuffer buffers;
        buffers.initBufferFromMeshData( meshdata );

        m_bbox.merge(meshdata.vertices);

        IRenderable* pRenderable1 = new SimpleRenderable(buffers);
        Global::renderWindow().addRenderable( pRenderable1 );
        m_wf_meshes.insert( pRenderable1 );

        IRenderable* pRenderable2 = new TriangleRenderable(buffers);
        Global::renderWindow().addRenderable( pRenderable2 );
        m_triagnle_meshes.insert( pRenderable2 );

        IRenderable* pRenderable3 = (IRenderable*)new TriangleWithVertexNormalRenderable(buffers);
        Global::renderWindow().addRenderable( pRenderable3 );
        m_smooth_triangle_meshes.insert( pRenderable3 );
    }

    AABBRender* pBoxrender = new AABBRender(m_bbox);
    m_otherMesh.insert( pBoxrender);
    Global::renderWindow().addRenderable( pBoxrender );


    Global::cameraControl().fitBox( m_bbox);

    showMeshGroup( m_otherMesh, false);
    displayAsSmoothLighting();

    m_pUI = new ModelDisplayUIView(this);
    Global::uiManager().addUIView( m_pUI);

    return true;
}


void ModelDisplay::displayAsPureColor()
{
    showMeshGroup(m_wf_meshes, true );
    showMeshGroup(m_triagnle_meshes, false );
    showMeshGroup(m_smooth_triangle_meshes, false );
}

void ModelDisplay::displayAsHardLighting()
{
    showMeshGroup(m_wf_meshes, false );
    showMeshGroup(m_triagnle_meshes, true );
    showMeshGroup(m_smooth_triangle_meshes, false );
}

void ModelDisplay::displayAsSmoothLighting()
{
    showMeshGroup(m_wf_meshes, false );
    showMeshGroup(m_triagnle_meshes, false );
    showMeshGroup(m_smooth_triangle_meshes, true );
}

void ModelDisplay::reloadShader() {
    Global::shaderMgr().reloadShader("tri_vn_pc");
    Global::shaderMgr().reloadShader("triangle");
}

bool ModelDisplay::update()
{
    return true;
}

bool ModelDisplay::destroy()
{
    removeMeshGroup(m_wf_meshes);
    removeMeshGroup(m_triagnle_meshes);
    removeMeshGroup(m_smooth_triangle_meshes);
    removeMeshGroup(m_otherMesh);

    Global::uiManager().removeUIView( m_pUI);
    delete m_pUI;
    m_pUI = nullptr;

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