//
// Created by sitan on 19-9-16.
//

#include "ShadowRender.h"
#include "global.h"
#include "shader.h"

int ShadowRender::addMeshBuffer( const MeshBuffer& meshBuffer )
{
    static int s_increaseID = 0;
    int retID = ++s_increaseID;

    m_meshes.insert( std::make_pair(retID, meshBuffer));
    return retID;
}

void ShadowRender::removeMeshBuffer( int shadowMeshId )
{
    auto it = m_meshes.find( shadowMeshId );
    if( it == m_meshes.end() )
        return;

    m_meshes.erase( it );
}

void ShadowRender::initialize()
{
    glGenFramebuffers(1, &m_frameBufferObj);

    glGenTextures(1,&m_dethTexture);
    glBindTexture(GL_TEXTURE_2D, m_dethTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_TEXTURE_WIDTH, DEPTH_TEXTURE_WIDTH, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT,  nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObj);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_dethTexture, 0 );
    //explicitly tell opengl that we don't use color buffer.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
    {
        std::cout << "Error: framebuffer is not complete." << std::endl;
        assert(false);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &m_vao);

    m_shader = Global::shaderMgr().getShader("shadow_depth");
    m_viewProjAttrLoc = glGetUniformLocation (m_shader->Id(), "viewprojMat");
}

void ShadowRender::destroy()
{
    glDeleteTextures(1,&m_dethTexture);
    glDeleteFramebuffers(1,&m_frameBufferObj);

    glDeleteVertexArrays(1, &m_vao);
    m_meshes.clear();

    m_shader = nullptr;
}


void ShadowRender::renderDepthTexture( const Matrix4& viewMatrix, const Matrix4& projMat )
{
    glViewport(0,0, DEPTH_TEXTURE_WIDTH, DEPTH_TEXTURE_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObj);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(m_vao);

    //shader, matrix
    glUseProgram(  m_shader->Id() );

    Matrix4  viewProjMat = projMat* viewMatrix;
    glUniformMatrix4fv( m_viewProjAttrLoc, 1, GL_FALSE ,(float*) &viewProjMat);


    for( auto it = m_meshes.begin(); it != m_meshes.end(); ++it )
    {
        MeshBuffer& buffer = it->second;

        glBindBuffer(GL_ARRAY_BUFFER, buffer.m_vbo_vertives.Id());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_ebo.Id() );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


        glDrawElements(GL_TRIANGLES, buffer.m_indicesCount, GL_UNSIGNED_INT,0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0 );

}


GLuint ShadowRender::getDepthTexture()
{
    return m_dethTexture;
}

