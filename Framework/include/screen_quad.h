//
// Created by Sitan Chen on 2019/9/4.
//

#ifndef ADVANCEDGL_SCREENQUAD_H
#define ADVANCEDGL_SCREENQUAD_H

#include "mesh_buffer.h"
#include "shader.h"
#include "irenderable.h"

class ScreenQuad
{
public:
    ScreenQuad();
    ~ScreenQuad();
    void render();

    GLuint getVAO()
    {
        return m_VAO;
    }

private:
    MeshBuffer m_buffers;
    GLuint     m_VAO = 0;
};

class ColorTextureRender : public  IRenderable
{
public:
    ColorTextureRender();

    void setColorTexture( GLuint textureID )
    {
        m_textureId = textureID;
    }

    void render()
    {
        if(m_textureId != 0)
        {
            glUseProgram( m_colorTextureDebugShader->Id());
            glBindTexture(GL_TEXTURE_2D, m_textureId);
            m_quad.render();
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
        }
    }
private:
    GLuint m_textureId = 0;
    ShaderObj* m_colorTextureDebugShader = nullptr;
    ScreenQuad m_quad;
};

#endif //ADVANCEDGL_SCREENQUAD_H
