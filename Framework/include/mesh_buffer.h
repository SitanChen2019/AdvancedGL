//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_MESH_BUFFER_H
#define ADVANCEDGL_MESH_BUFFER_H

#include "render_type.h"
#include "globject.h"


struct MeshBuffer
{
    SPGLBuffer m_vbo_vertives ;
    SPGLBuffer m_vbo_normals ;
    SPGLBuffer m_vbo_textcoords ;
    SPGLBuffer m_ebo ;

    PrimitiveType m_primitveType = PT_TRIANGLE;

    GLuint m_indicesCount = 0;
    GLuint m_vertexCount = 0;
    GLuint m_normalCount = 0;
public:
    void initBufferFromMeshData( const MeshData& meshData );
};

#endif //ADVANCEDGL_MESH_BUFFER_H
