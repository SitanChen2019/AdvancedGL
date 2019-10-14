//
// Created by sitan on 19-9-12.
//

#include "utilities.h"
#include <iostream>

 void Utilities::calculateSmoothNormal(  const  Vec3Sequence& vertices,
                                    const Int32Sequence& indices,
                                    Vec3Sequence&  normals )
 {
    std::cout << " calculateSmoothNormal , vertex size : " << vertices.size()
    << " indices size " << indices.size() << "." << std::endl;

    normals.clear();
    normals.resize( vertices.size(), Vec3(0,0,0));

    Vector<int> normalCountPerVertex;
    normalCountPerVertex.resize( vertices.size(), 0);

    for( size_t i = 0, n = indices.size(); i<n; i+=3 )
    {
        const int vid0 =  indices[i];
        const int vid1 =  indices[i+1];
        const int vid2 =  indices[i+2];

        const Vec3& v0 = vertices[ vid0 ];
        const Vec3& v1 = vertices[ vid1 ];
        const Vec3& v2 = vertices[ vid2 ];

        Vec3 normal = glm::normalize( glm::cross( v1-v0, v2-v0) );

        normals[vid0] += normal;
        normals[vid1] += normal;
        normals[vid2] += normal;

        normalCountPerVertex[vid0] += 1;
        normalCountPerVertex[vid1] += 1;
        normalCountPerVertex[vid2] += 1;
    }

    for( size_t i =0 , n = vertices.size(); i < n; ++i )
    {
        if( normalCountPerVertex[i] > 1 )
        {
            normals[i] /= normalCountPerVertex[i];
            normals[i] = glm::normalize(normals[i]);
        }
    }
 }