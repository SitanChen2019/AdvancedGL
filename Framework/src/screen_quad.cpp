//
// Created by Sitan Chen on 2019/9/4.
//

#include "screen_quad.h"
#include "render_type.h"
#include "global.h"

ScreenQuad::ScreenQuad() {

    MeshData meshData;
    meshData.vertices = Vec3Sequence{
            Vec3{-1.f,-1.f,0},
            Vec3{-1.f,1.f,0},
            Vec3{1.f,1.f,0},
            Vec3{1.f,-1.f,0}
    };

    meshData.textCoors = Vec2Sequence{
            Vec2{0,0},
            Vec2{0,1.f},
            Vec2{1.f,1.f},
            Vec2{1.0,0}
    };

    meshData.indices = Int32Sequence{
            1,0,3,
            1,3,2
    };

    m_buffers.initBufferFromMeshData( meshData );

    glGenVertexArrays(1,&m_VAO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_textcoords.Id() );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id() );
    glBindVertexArray(0);
}

void ScreenQuad::render() {
    glBindVertexArray( m_VAO );
    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount,GL_UNSIGNED_INT , 0);
    glBindVertexArray(0);
}

ScreenQuad::~ScreenQuad()
{
    glDeleteVertexArrays(1, &m_VAO);
};

ColorTextureRender::ColorTextureRender()
:IRenderable(255)
{
    Global::shaderMgr().registerShader("color-texture-debug", "../res/shader/quad.vs","../res/shader/quad.fs");
    m_colorTextureDebugShader = Global::shaderMgr().getShader("color-texture-debug");
};

