//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_MODELDISPLAY_H
#define ADVANCEDGL_MODELDISPLAY_H

#include "idemo.h"
#include <string>
#include "screen_quad.h"
#include "aabb.h"



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
