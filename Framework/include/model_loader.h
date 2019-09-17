//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_MODELLOADER_H
#define ADVANCEDGL_MODELLOADER_H

#include "render_type.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <string>

class ModelLoader
{
public:
    static Vector<MeshData> loadModel( std::string path )
    {
        Vector<MeshData> retlist;

        Assimp::Importer importer;
		unsigned flags = 0;
		flags |= aiProcess_Triangulate;
		flags |= aiProcess_GenNormals;
		//flags |= aiProcess_JoinIdenticalVertices;
        const aiScene* pScene = importer.ReadFile(path, flags);
		if (pScene == nullptr)
		{
			std::cout << "Warning : load nothing from " << path << std::endl;
			return  retlist;
		}


        Vec3Sequence vertices;
        Int32Sequence indices;
        Vec3Sequence normals;

        unsigned totalVertexCount(0);
        unsigned totalIndicesCount(0);
        unsigned totalNormalCount(0);


        for (unsigned i = 0; i < pScene->mNumMeshes; ++i) {

            const aiMesh *pMesh = pScene->mMeshes[i];

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

        std::cout << " Loaded model : " << path << std::endl;
        std::cout << "Vertex Count : " << totalVertexCount  << std::endl;
        std::cout << "Indices Count : " << totalIndicesCount  << std::endl;
        std::cout << "Normal Count : " << totalNormalCount  << std::endl;
        return retlist;
    }
};
#endif //ADVANCEDGL_MODELLOADER_H
