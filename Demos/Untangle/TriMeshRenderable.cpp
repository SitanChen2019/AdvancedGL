#include "global.h"
#include "TriMeshRenderable.h"

TriMeshRenderable::TriMeshRenderable(MeshBuffer buffers)
	:IRenderable(50)
	, m_buffers(buffers)
{

}

void TriMeshRenderable::attach() {
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
	glBindVertexArray(0);

	glBindVertexArray(0);

	m_triangleShader = Global::shaderMgr().getShader("triangle");

	m_diffuseAttrLoc = glGetUniformLocation(m_triangleShader->Id(), "diffMat");
	m_viewAttrLoc = glGetUniformLocation(m_triangleShader->Id(), "view");
	m_projAttrLoc = glGetUniformLocation(m_triangleShader->Id(), "projection");

	//assert( m_diffuseAttrLoc != -1);
	assert(m_viewAttrLoc != -1);
	assert(m_projAttrLoc != -1);

}

void TriMeshRenderable::detach() {
	glDeleteVertexArrays(1, &m_vao);

	m_triangleShader = nullptr;
}

void  TriMeshRenderable::render() {
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	Matrix4 mat = Global::renderWindow().getProjViewMatrix();

	glUseProgram(m_triangleShader->Id());
	glBindVertexArray(m_vao);

	Matrix4  viewMat = Global::renderWindow().getViewMatrix();
	glUniformMatrix4fv(m_viewAttrLoc, 1, GL_FALSE, (float*)&viewMat);

	Matrix4  projMat = Global::renderWindow().getProjMatrix();
	glUniformMatrix4fv(m_projAttrLoc, 1, GL_FALSE, (float*)&projMat);

	glUniform3f(m_diffuseAttrLoc, m_color.x, m_color.y, m_color.z);

	glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void  TriMeshRenderable::updateVertices(const Vec3Sequence& vertices)
{
	assert(vertices.size() == m_buffers.m_vertexCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id());
	Vec3* pPos = (Vec3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (pPos != nullptr)
	{
		memcpy_s(pPos, sizeof(Vec3) * vertices.size(), vertices.data(), sizeof(Vec3) * vertices.size());
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
