//
// Created by Sitan Chen on 2019/9/28.
//

#ifndef ADVANCEDGL_TESSELLATIONSHADER_H
#define ADVANCEDGL_TESSELLATIONSHADER_H

#include "idemo.h"

class BesizerRenderable;


class TessellationShader : public IDemo 
{
public:
    bool init() override;
    bool update() override;
    bool destroy()override;

public:
    BesizerRenderable* m_bezier;

};


#endif //ADVANCEDGL_TESSELLATIONSHADER_H
