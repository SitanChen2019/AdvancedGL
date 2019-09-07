//
// Created by Sitan Chen on 2019/9/3.
//

#include "ModelDisplay.h"
#include "global.h"
#include "model_loader.h"

WireFrameRenderable::WireFrameRenderable(MeshBuffer buffers)
:IRenderable(50)
,m_buffers(buffers)
{

}

void WireFrameRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindVertexArray(0);

    m_wireFrameShader = Global::shaderMgr().getShader("simple");

    m_colorAttrLoc = glGetUniformLocation(m_wireFrameShader->Id(), "color" );
    m_viewprojMatAttrLoc = glGetUniformLocation (m_wireFrameShader->Id(), "viewprojMat");

    assert( m_colorAttrLoc != -1);
    assert( m_viewprojMatAttrLoc != -1);

}

void WireFrameRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_wireFrameShader = nullptr;
}

void WireFrameRenderable::render() {

    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_wireFrameShader->Id() );
    glBindVertexArray(m_vao);

    Matrix4  viewprojMat = Global::renderWindow().getProjViewMatrix();
    glUniformMatrix4fv( m_viewprojMatAttrLoc, 1, GL_FALSE ,(float*) &viewprojMat);

    glUniform3f( m_colorAttrLoc, m_color.x, m_color.y, m_color.z );
    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

bool ModelDisplay::init() {

    Vector<MeshData> tessellationDatas = ModelLoader::loadModel("../res/models/Tshirt_01/Tshirt_obj.obj");

    for( auto& meshdata : tessellationDatas )
    {
        MeshBuffer buffers;
        buffers.initBufferFromMeshData( meshdata );

        m_bbox.merge(meshdata.vertices);

        WireFrameRenderable* pRenderable = new WireFrameRenderable(buffers);

        m_meshes.insert( pRenderable);
        Global::renderWindow().addRenderable( pRenderable );
    }

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