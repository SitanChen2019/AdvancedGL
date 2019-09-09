//
// Created by Sitan Chen on 2019/9/9.
//

#include "aabb.h"
#include "irenderable.h"
#include "mesh_buffer.h"
#include "shader.h"
#include "global.h"

class AABBRender: public IRenderable
{
public:
    AABBRender(AABB& box)
    :IRenderable()
    {
        m_box = box;
    }


    void attach() override
    {
        m_shader = Global::shaderMgr().getShader("simple");
        m_colorAttrLoc = glGetUniformLocation(m_shader->Id(), "color" );
        m_viewprojMatAttrLoc = glGetUniformLocation (m_shader->Id(), "viewprojMat");

        glGenVertexArrays(1,&m_vao);

        MeshData data;
        AABB::generateTesellationData( &m_box, data.vertices, data.indices);
        data.primType = PT_LINES;

        m_buffer.initBufferFromMeshData( data );

        glBindVertexArray(m_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer.m_ebo.Id());

        glBindVertexArray(0);

    }

    void detach() override
    {
        glDeleteVertexArrays(1,&m_vao);
        m_vao = 0;
    }

    void render() override
    {
        glBindVertexArray(m_vao);
        glUseProgram(m_shader->Id());

        Matrix4  viewprojMat = Global::renderWindow().getProjViewMatrix();
        glUniformMatrix4fv( m_viewprojMatAttrLoc, 1, GL_FALSE ,(float*) &viewprojMat);

        glUniform3f( m_colorAttrLoc, m_color.x, m_color.y, m_color.z );

        glDrawElements(GL_LINES, m_buffer.m_indicesCount,GL_UNSIGNED_INT,0);

        glBindVertexArray(0);
    };

private:
    AABB m_box;
    GLuint  m_vao = 0;
    ShaderObj* m_shader = nullptr;
    MeshBuffer m_buffer;

    GLint m_colorAttrLoc = -1;
    GLint m_viewprojMatAttrLoc = -1;

    Vec3 m_color = Vec3(1,1,1);
};
