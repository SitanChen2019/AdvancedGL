//
// Created by sitan on 19-9-12.
//
#include "ModelDisplayRenderables.h"
#include "global.h"

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
    glBindVertexArray(0);

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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_wireFrameShader->Id() );
    glBindVertexArray(m_vao);

    Matrix4  viewprojMat = Global::renderWindow().getProjViewMatrix();
    glUniformMatrix4fv( m_viewprojMatAttrLoc, 1, GL_FALSE ,(float*) &viewprojMat);

    glUniform3f( m_colorAttrLoc, m_color.x, m_color.y, m_color.z );
    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_CULL_FACE);
}

TriangleRenderable::TriangleRenderable(MeshBuffer buffers)
        :IRenderable(50)
        ,m_buffers(buffers)
{

}

void TriangleRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
    glBindVertexArray(0);

    glBindVertexArray(0);

    m_triangleShader = Global::shaderMgr().getShader("triangle");

    m_diffuseAttrLoc = glGetUniformLocation(m_triangleShader->Id(), "diffMat" );
    m_viewAttrLoc = glGetUniformLocation (m_triangleShader->Id(), "view");
    m_projAttrLoc = glGetUniformLocation (m_triangleShader->Id(), "projection");

    //assert( m_diffuseAttrLoc != -1);
    assert( m_viewAttrLoc != -1);
    assert( m_projAttrLoc != -1);

}

void TriangleRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_triangleShader = nullptr;
}

void  TriangleRenderable::render() {
    glEnable(GL_DEPTH_TEST);


    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_triangleShader->Id() );
    glBindVertexArray(m_vao);

    Matrix4  viewMat = Global::renderWindow().getViewMatrix();
    glUniformMatrix4fv( m_viewAttrLoc, 1, GL_FALSE ,(float*) &viewMat);

    Matrix4  projMat = Global::renderWindow().getProjMatrix();
    glUniformMatrix4fv( m_projAttrLoc, 1, GL_FALSE ,(float*) &projMat);

    glUniform3f( m_diffuseAttrLoc, m_color.x, m_color.y, m_color.z );

    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}