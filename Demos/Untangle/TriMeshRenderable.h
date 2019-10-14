#ifndef TRIMESH_RENDERABLE_H
#define TRIMESH_RENDERABLE_H

#include "mesh_buffer.h"
#include "shader.h"
#include "irenderable.h"


class TriMeshRenderable : public IRenderable
{
public:
	TriMeshRenderable(MeshBuffer buffers);

	void updateVertices(const Vec3Sequence& vertices);

	void setDiffuse(Vec3 color) { m_color = color; }
	void attach() override;
	void render() override;
	void detach() override;



private:
	ShaderObj* m_triangleShader = nullptr;

	MeshBuffer m_buffers;
	GLint m_diffuseAttrLoc = -1;
	GLint m_viewAttrLoc = -1;
	GLint m_projAttrLoc = -1;
	GLuint m_vao = 0;

	Vec3 m_color = COLOR_WHITE;
};


#endif