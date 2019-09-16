//
// Created by sitan on 19-9-16.
//

#ifndef ADVANCEDGL_SHADOWRENDER_H
#define ADVANCEDGL_SHADOWRENDER_H

#include "mesh_buffer.h"
#include "shader.h"

class ShadowRender {

public:
    int addMeshBuffer( const MeshBuffer& meshBuffer );
    void removeMeshBuffer( int );

    void initialize();
    void renderDepthTexture( const Matrix4& viewMatrix, const Matrix4& projMat );
    void destroy();

    GLuint getDepthTexture();

private:
    const GLsizei DEPTH_TEXTURE_WIDTH =  1024;
    const GLsizei DEPTH_TEXTURE_HEIGHT = 1024;

    GLuint  m_frameBufferObj = 0;
    GLuint  m_dethTexture = 0;
    ShaderObj* m_shader = nullptr;
    GLuint  m_vao = 0;
    GLint    m_viewProjAttrLoc = -1;
    std::map<int,MeshBuffer> m_meshes;
};


#endif //ADVANCEDGL_SHADOWRENDER_H
