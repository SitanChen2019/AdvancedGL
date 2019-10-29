//
// Created by sitan on 19-9-12.
//
#include "TriangulationRenderables.h"
#include "global.h"

PointRenderable::PointRenderable(MeshBuffer buffers)
        :IRenderable(50)
        ,m_buffers(buffers)
{

}

void PointRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
    glBindVertexArray(0);

    glBindVertexArray(0);

    m_shader = Global::shaderMgr().getShader("simple");

    m_colorAttrLoc = glGetUniformLocation(m_shader->Id(), "color" );
    m_viewprojMatAttrLoc = glGetUniformLocation (m_shader->Id(), "viewprojMat");

    assert( m_colorAttrLoc != -1);
    assert( m_viewprojMatAttrLoc != -1);

}

void PointRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_shader = nullptr;
}

void PointRenderable::render() {
	glPointSize(10.0f);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_shader->Id() );
    glBindVertexArray(m_vao);

    Matrix4  viewprojMat = Global::renderWindow().getProjViewMatrix();
    glUniformMatrix4fv( m_viewprojMatAttrLoc, 1, GL_FALSE ,(float*) &viewprojMat);

    glUniform3f( m_colorAttrLoc, m_color.x, m_color.y, m_color.z );
    glDrawElements(GL_POINTS, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_CULL_FACE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}


LineRenderable::LineRenderable(MeshBuffer buffers)
	:IRenderable(50)
	, m_buffers(buffers)
{

}

void LineRenderable::attach() {

	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
	glBindVertexArray(0);

	glBindVertexArray(0);

	m_shader = Global::shaderMgr().getShader("simple");

	m_colorAttrLoc = glGetUniformLocation(m_shader->Id(), "color");
	m_viewprojMatAttrLoc = glGetUniformLocation(m_shader->Id(), "viewprojMat");

	assert(m_colorAttrLoc != -1);
	assert(m_viewprojMatAttrLoc != -1);

}

void LineRenderable::detach() {
	glDeleteVertexArrays(1, &m_vao);

	m_shader = nullptr;
}

void LineRenderable::render() {
	glLineWidth(5.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Matrix4 mat = Global::renderWindow().getProjViewMatrix();

	glUseProgram(m_shader->Id());
	glBindVertexArray(m_vao);

	Matrix4  viewprojMat = Global::renderWindow().getProjViewMatrix();
	glUniformMatrix4fv(m_viewprojMatAttrLoc, 1, GL_FALSE, (float*)&viewprojMat);

	glUniform3f(m_colorAttrLoc, m_color.x, m_color.y, m_color.z);
	glDrawElements(GL_LINES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
