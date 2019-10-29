//
// Created by sitan on 19-9-12.
//

#ifndef TRIANGULATION_RENDERABLES_H
#define TRIANGULATION_RENDERABLES_H

#include "mesh_buffer.h"
#include "shader.h"
#include "irenderable.h"

class PointRenderable : public IRenderable {
public:
	PointRenderable( MeshBuffer buffers );

    void setColor( Vec3 color) { m_color = color; }
    void attach() override;
    void render() override;
    void detach() override;

private:
    ShaderObj* m_shader;

    MeshBuffer m_buffers;
    GLint m_colorAttrLoc;
    GLint m_viewprojMatAttrLoc;
    GLuint m_vao;

    Vec3 m_color = COLOR_WHITE ;
};

class LineRenderable : public IRenderable {
public:
	LineRenderable(MeshBuffer buffers);

	void setColor(Vec3 color) { m_color = color; }
	void attach() override;
	void render() override;
	void detach() override;

private:
	ShaderObj* m_shader;

	MeshBuffer m_buffers;
	GLint m_colorAttrLoc;
	GLint m_viewprojMatAttrLoc;
	GLuint m_vao;

	Vec3 m_color = COLOR_WHITE;
};

#endif //TRIANGULATION_RENDERABLES_H
