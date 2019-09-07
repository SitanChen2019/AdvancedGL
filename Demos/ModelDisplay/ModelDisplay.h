//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_MODELDISPLAY_H
#define ADVANCEDGL_MODELDISPLAY_H

#include "idemo.h"
#include <string>
#include "screen_quad.h"
#include "aabb.h"

class WireFrameRenderable : public IRenderable {
public:
    WireFrameRenderable( MeshBuffer buffers );

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


class ModelDisplay : public IDemo
{
public:
    virtual ~ModelDisplay(){}
    virtual bool init();
    virtual bool update();
    virtual bool destroy();

public:
    std::set<IRenderable*> m_meshes;
    AABB                   m_bbox;
};


#endif //ADVANCEDGL_MODELDISPLAY_H
