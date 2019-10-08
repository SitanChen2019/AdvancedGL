//
// Created by Sitan Chen on 2019/9/28.
//

#include "TessellationShader.h"
#include "BesizerRenderable.h"
#include "global.h"

bool TessellationShader::init()
{
    m_bezier = new BesizerRenderable();
    Global::renderWindow().addRenderable( m_bezier );
    return  true;
}

bool TessellationShader::update()
{
    return  true;
}
bool TessellationShader::destroy()
{
    Global::renderWindow().removeRenderable( m_bezier );
    return true;
}