//
// Created by Sitan Chen on 2019/9/3.
//

#include "ModelDisplay.h"
#include "global.h"
#include "model_loader.h"
#include "aabb_render.h"

#include "ModelDisplayRenderables.h"


bool ModelDisplay::init() {

    Vector<MeshData> tessellationDatas = ModelLoader::loadModel("../res/models/Tshirt_01/Tshirt_obj.obj");

    for( auto& meshdata : tessellationDatas )
    {
        MeshBuffer buffers;
        buffers.initBufferFromMeshData( meshdata );

        m_bbox.merge(meshdata.vertices);

        //WireFrameRenderable* pRenderable = new WireFrameRenderable(buffers);
        TriangleRenderable* pRenderable = new TriangleRenderable(buffers);

        m_meshes.insert( pRenderable);
        Global::renderWindow().addRenderable( pRenderable );
    }

    AABBRender* pBoxrender = new AABBRender(m_bbox);
    m_meshes.insert( pBoxrender);
    Global::renderWindow().addRenderable( pBoxrender );

    Global::cameraControl().fitBox( m_bbox);
    return true;
}


bool ModelDisplay::update()
{
    return true;
}

bool ModelDisplay::destroy()
{
    for( auto pRenderable : m_meshes )
        Global::renderWindow().removeRenderable( pRenderable );

    m_meshes.clear();
    return true;
}