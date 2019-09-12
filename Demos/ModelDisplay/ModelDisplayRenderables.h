//
// Created by sitan on 19-9-12.
//

#ifndef ADVANCEDGL_MODELDISPLAYRENDERABLES_H
#define ADVANCEDGL_MODELDISPLAYRENDERABLES_H

#include "mesh_buffer.h"
#include "shader.h"
#include "irenderable.h"

class SimpleRenderable : public IRenderable {
public:
    SimpleRenderable( MeshBuffer buffers );

    void setColor( Vec3 color) { m_color = color; }
    void attach() override;
    void render() override;
    void detach() override;

private:
    ShaderObj* m_wireFrameShader;

    MeshBuffer m_buffers;
    GLint m_colorAttrLoc;
    GLint m_viewprojMatAttrLoc;
    GLuint m_vao;

    Vec3 m_color = COLOR_WHITE ;
};

class TriangleRenderable : public  IRenderable {
public:
    TriangleRenderable( MeshBuffer buffers );

    void setDiffuse( Vec3 color) { m_color = color; }
    void attach() override;
    void render() override;
    void detach() override;

private:
    ShaderObj* m_triangleShader;

    MeshBuffer m_buffers;
    GLint m_diffuseAttrLoc;
    GLint m_viewAttrLoc;
    GLint m_projAttrLoc;
    GLuint m_vao;

    Vec3 m_color = COLOR_WHITE ;
};

class TriangleWithVertexNormalRenderable : public  IRenderable {
public:
    TriangleWithVertexNormalRenderable( MeshBuffer buffers );

    void setDiffuse( Vec3 color) { m_color = color; }
    void attach() override;
    void render() override;
    void detach() override;

private:
    ShaderObj* m_shader;

    MeshBuffer m_buffers;
    GLint m_diffuseAttrLoc;
    GLint m_viewAttrLoc;
    GLint m_projAttrLoc;
    GLuint m_vao;

    Vec3 m_color = COLOR_WHITE ;
};

#endif //ADVANCEDGL_MODELDISPLAYRENDERABLES_H
