//
// Created by sitan on 19-9-12.
//
#include "ModelDisplayRenderables.h"
#include "global.h"

SimpleRenderable::SimpleRenderable(MeshBuffer buffers)
        :IRenderable(50)
        ,m_buffers(buffers)
{

}

void SimpleRenderable::attach() {
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

void SimpleRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_wireFrameShader = nullptr;
}

void SimpleRenderable::render() {
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

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
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
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
    //glEnable(GL_CULL_FACE);

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


TriangleWithVertexNormalRenderable::TriangleWithVertexNormalRenderable(MeshBuffer buffers)
        :IRenderable(50)
        ,m_buffers(buffers)
{
    assert( buffers.m_normalCount == buffers.m_vertexCount);
}

void TriangleWithVertexNormalRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_normals.Id() );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());

    glBindVertexArray(0);

    m_shader = Global::shaderMgr().getShader("tri_vn_pc");

    m_diffuseAttrLoc = glGetUniformLocation(m_shader->Id(), "diffMat" );
    m_viewAttrLoc = glGetUniformLocation (m_shader->Id(), "view");
    m_projAttrLoc = glGetUniformLocation (m_shader->Id(), "projection");

    //assert( m_diffuseAttrLoc != -1);
    assert( m_viewAttrLoc != -1);
    assert( m_projAttrLoc != -1);

}

void TriangleWithVertexNormalRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_shader = nullptr;
}

void  TriangleWithVertexNormalRenderable::render() {
    glEnable(GL_DEPTH_TEST);


    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_shader->Id() );
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


TriangleWithVertexNormalShadowRenderable::TriangleWithVertexNormalShadowRenderable(MeshBuffer buffers)
        :IRenderable(50)
        ,m_buffers(buffers)
{
    assert( buffers.m_normalCount == buffers.m_vertexCount);
}

void TriangleWithVertexNormalShadowRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_normals.Id() );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());

    glBindVertexArray(0);

    m_shader = Global::shaderMgr().getShader("tri_vn_shadow");

    m_diffuseAttrLoc = glGetUniformLocation(m_shader->Id(), "diffMat" );
    m_viewAttrLoc = glGetUniformLocation (m_shader->Id(), "view");
    m_projAttrLoc = glGetUniformLocation (m_shader->Id(), "projection");
    m_lightViewProjAttrLoc = glGetUniformLocation (m_shader->Id(), "lightViewProj");

    //assert( m_diffuseAttrLoc != -1);
    assert( m_viewAttrLoc != -1);
    assert( m_projAttrLoc != -1);
    assert(m_lightViewProjAttrLoc!=-1);

}

void TriangleWithVertexNormalShadowRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_shader = nullptr;
}

void  TriangleWithVertexNormalShadowRenderable::render() {
    glEnable(GL_DEPTH_TEST);


    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_shader->Id() );
    glBindVertexArray(m_vao);

    Matrix4  viewMat = Global::renderWindow().getViewMatrix();
    glUniformMatrix4fv( m_viewAttrLoc, 1, GL_FALSE ,(float*) &viewMat);

    Matrix4  projMat = Global::renderWindow().getProjMatrix();
    glUniformMatrix4fv( m_projAttrLoc, 1, GL_FALSE ,(float*) &projMat);

    glUniformMatrix4fv( m_lightViewProjAttrLoc,1, GL_FALSE, (float*)(&m_lightViewProjMat));

    glBindTexture(GL_TEXTURE_2D, m_shadowTextureId);
    glActiveTexture(GL_TEXTURE0 );

    glUniform3f( m_diffuseAttrLoc, m_color.x, m_color.y, m_color.z );

    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}