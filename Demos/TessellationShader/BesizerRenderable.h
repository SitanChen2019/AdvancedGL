//
// Created by Sitan Chen on 2019/9/30.
//

#ifndef ADVANCEDGL_BESIZERRENDERABLE_H
#define ADVANCEDGL_BESIZERRENDERABLE_H

#include "irenderable.h"
#include "render_type.h"
#include "aabb.h"
#include "screen_quad.h"

class BesizerRenderable  : public  IRenderable {
    void attach() override ;
    void detach() override ;
    void render() override ;

private:
    unsigned m_BezierProgram;

    unsigned  m_vao;
    unsigned  m_vbo;
    unsigned  m_ebo;

    unsigned  m_indicesCount;
    AABB m_box;
};


#endif //ADVANCEDGL_BESIZERRENDERABLE_H
