//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_MODELLOADER_H
#define ADVANCEDGL_MODELLOADER_H

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>
#include <string>


#include "render_type.h"
#include "animator.h"

//#define  CST_DEBUG

class ModelLoader
{
public:
    ModelLoader( std::string path )
    {
        m_path = path;

        unsigned flags = 0;
        flags |= aiProcess_Triangulate;
        flags |= aiProcess_GenNormals;
        //flags |= aiProcess_JoinIdenticalVertices;
        m_pScene = m_importer.ReadFile(path, flags);
        if (m_pScene == nullptr)
        {
            std::cout << "Warning : failed to load scene from " << path << std::endl;
        }
    }

    ~ModelLoader()
    {

    }

    const aiScene* getScene()
    {
        return m_pScene;
    }

    Vector<MeshData> loadModel()
    {
        Vector<MeshData> retlist;

		if (m_pScene == nullptr)
		{
			return  retlist;
		}


        Vec3Sequence vertices;
        Int32Sequence indices;
        Vec3Sequence normals;

        unsigned totalVertexCount(0);
        unsigned totalIndicesCount(0);
        unsigned totalNormalCount(0);


        for (unsigned i = 0; i < m_pScene->mNumMeshes; ++i) {

            const aiMesh *pMesh = m_pScene->mMeshes[i];

            vertices.clear();
            vertices.resize(pMesh->mNumVertices);
            memcpy(vertices.data(), pMesh->mVertices, sizeof(Vec3) * pMesh->mNumVertices);


            indices.clear();
            int indicesNumber = 0;
            for (unsigned j = 0; j < pMesh->mNumFaces; ++j) {
                indicesNumber += pMesh->mFaces[j].mNumIndices;
            }

            indices.resize(indicesNumber);
            unsigned *pDest = (unsigned *) indices.data();
            for (unsigned j = 0; j < pMesh->mNumFaces; ++j) {
                unsigned indicesCount = pMesh->mFaces[j].mNumIndices;
                memcpy(pDest, pMesh->mFaces[j].mIndices, sizeof(unsigned) * indicesCount);
                pDest += indicesCount;
            }

            normals.clear();
            if (pMesh->HasNormals()) {
                normals.resize(pMesh->mNumVertices);
                memcpy(normals.data(), pMesh->mNormals, sizeof(Vec3) * pMesh->mNumVertices);
            }

            MeshData meshData;
            meshData.vertices.swap( vertices );
            meshData.indices.swap( indices );
            meshData.normals.swap( normals );
            meshData.primType = PT_TRIANGLE;

            totalVertexCount += meshData.vertices.size();
            totalIndicesCount += meshData.indices.size();
            totalNormalCount += meshData.normals.size();

            retlist.push_back( meshData );
        }

        std::cout << " Loaded model : " << m_path << std::endl;
        std::cout << "Vertex Count : " << totalVertexCount  << std::endl;
        std::cout << "Indices Count : " << totalIndicesCount  << std::endl;
        std::cout << "Normal Count : " << totalNormalCount  << std::endl;


        return retlist;
    }



private:
    const aiScene* m_pScene = nullptr;
    std::string m_path;

    Assimp::Importer m_importer;
};

#endif //ADVANCEDGL_MODELLOADER_H
