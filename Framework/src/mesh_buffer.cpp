//
// Created by Sitan Chen on 2019/9/3.
//

#include "mesh_buffer.h"
#include "global.h"


void MeshBuffer::initBufferFromMeshData(const MeshData &meshData){
    m_vbo_vertives = Global::glObjMgr().createBufferObject();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertives.Id());
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*meshData.vertices.size(), meshData.vertices.data(), GL_STATIC_DRAW);


    if ( meshData.normals.empty() == false)
    {
        m_vbo_normals = Global::glObjMgr().createBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals.Id());
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*meshData.normals.size(), meshData.normals.data(), GL_STATIC_DRAW);
    }

    if( meshData.indices.empty() == false )
    {
        m_ebo = Global::glObjMgr().createBufferObject();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo.Id());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*meshData.indices.size(),  meshData.indices.data(), GL_STATIC_DRAW);
    }

    if( meshData.textCoors.empty() == false)
    {
        m_vbo_textcoords = Global::glObjMgr().createBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_textcoords.Id());
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2)*meshData.textCoors.size(), meshData.textCoors.data(), GL_STATIC_DRAW);
    }


    m_vertexCount = (unsigned)meshData.vertices.size();
    m_indicesCount = (unsigned)meshData.indices.size();
}

