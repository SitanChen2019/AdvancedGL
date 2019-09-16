//
// Created by Sitan Chen on 2019/9/7.
//

#ifndef ADVANCEDGL_SHAPE_H
#define ADVANCEDGL_SHAPE_H

#include "render_type.h"

class Shape
{
public:
    static MeshData createOXYTriangle()
    {
        MeshData ret;

        ret.vertices.push_back( Vec3(0,0,0));
        ret.vertices.push_back( Vec3(1,0,0));
        ret.vertices.push_back( Vec3(0,1,0));

        ret.primType = PT_TRIANGLE;

        ret.indices.push_back(0);
        ret.indices.push_back(1);
        ret.indices.push_back(2);

        return ret;
    }

    static Vec3 sphereCoord2EulerCoord_Degree( float r, float latitudeAngle, float longitudeAngle)
    {
         Vec3 ret;

         ret.x = r*cos( glm::radians(latitudeAngle)) * cos( glm::radians(longitudeAngle));
         ret.y = r*sin( glm::radians(latitudeAngle));
         ret.z = r*cos( glm::radians(latitudeAngle)) * sin( glm::radians(longitudeAngle));

         return ret;
    }

    static MeshData createSphere( float r = 1 , int latitudePart = 10, int longitudePart = 20)
    {
        MeshData ret;
        assert( latitudePart >= 2);
        assert( longitudePart >= 2);

        ret.vertices.reserve( 1 + 1 + (latitudePart + 1 - 2)*longitudePart);

        float latitudeDelta = 180.0f/latitudePart;
        for( int i = 0; i <= latitudePart; ++i)
        {
            float latitudeAngle = 0 - i*latitudeDelta + 90;
            if( i == 0 || i == latitudePart )
            {
                //only one vertex at the top and bottom
                ret.vertices.push_back( sphereCoord2EulerCoord_Degree(r,latitudeAngle,0));
            }
            else
            {
                float longitudeDelta = 360.0f/longitudePart;
                for( int j = 0; j < longitudePart; ++j )
                {
                    float longitudeAngle = j*longitudeDelta ;
                    ret.vertices.push_back( sphereCoord2EulerCoord_Degree(r,latitudeAngle,longitudeAngle));
                }
            }
        }

        int vertex_start_index = 0;
        int current_line_vertex_count = 0;
        int next_line_vertex_count = 0;
        for( int i = 0; i < latitudePart; ++i)
        {
            current_line_vertex_count = longitudePart;
            next_line_vertex_count = longitudePart;

            if( i == 0 )
            {
                current_line_vertex_count = 1;
            }

            if( i == latitudePart -1 )
            {
                next_line_vertex_count = 1;
            }

            if( current_line_vertex_count == 1 and next_line_vertex_count == 1)
            {
                assert(false);
            }

            if( current_line_vertex_count == 1)
            {
                for( int j = 0; j< longitudePart; ++j )
                {
                    ret.indices.push_back( vertex_start_index + current_line_vertex_count + j);
                    ret.indices.push_back( vertex_start_index );
                    ret.indices.push_back( vertex_start_index + current_line_vertex_count + ((j+1)%longitudePart) );


                }
            }
            else if( next_line_vertex_count == 1 )
            {
                for( int j = 0; j< longitudePart; ++j )
                {
                    ret.indices.push_back( vertex_start_index  + j);
                    ret.indices.push_back( vertex_start_index + ((j+1)%longitudePart) );
                    ret.indices.push_back( vertex_start_index + current_line_vertex_count );

                }
            }
            else
            {
                for( int j = 0; j< longitudePart; ++j )
                {
                    ret.indices.push_back( vertex_start_index  + j);
                    ret.indices.push_back( vertex_start_index + current_line_vertex_count + ((j+1)%longitudePart) );
                    ret.indices.push_back( vertex_start_index + current_line_vertex_count + j);


                    ret.indices.push_back( vertex_start_index  + j);
                    ret.indices.push_back( vertex_start_index + ((j+1)%longitudePart) );
                    ret.indices.push_back( vertex_start_index + current_line_vertex_count + ((j+1)%longitudePart) );

                }
            }


            vertex_start_index += current_line_vertex_count;
        }

        return ret;
    }


    static MeshData createXZRectangle( float width, float height )
    {
        MeshData ret;

        auto width_dir = Vec3(1,0,0);
        auto height_dir = Vec3(0,0,1);
        Vec3 center = Vec3(0,0,0);

        Vec3 v0 = center - width/2 * width_dir  + height/2*height_dir;
        Vec3 v1 = center + width/2 * width_dir  + height/2*height_dir;
        Vec3 v2 = center - width/2 * width_dir  - height/2*height_dir;
        Vec3 v3 = center + width/2 * width_dir  - height/2*height_dir;

        ret.vertices.push_back(v0);
        ret.vertices.push_back(v1);
        ret.vertices.push_back(v2);
        ret.vertices.push_back(v3);

        ret.indices.push_back(0);
        ret.indices.push_back(3);
        ret.indices.push_back(2);

        ret.indices.push_back(0);
        ret.indices.push_back(1);
        ret.indices.push_back(3);

        ret.normals.push_back(Vec3(0,1,0));
        ret.normals.push_back(Vec3(0,1,0));
        ret.normals.push_back(Vec3(0,1,0));
        ret.normals.push_back(Vec3(0,1,0));

        ret.textCoors.push_back(glm::vec2(0,0));
        ret.textCoors.push_back(glm::vec2(1,0));
        ret.textCoors.push_back(glm::vec2(0,1));
        ret.textCoors.push_back(glm::vec2(1,1));

        return ret;
    }

    static void Translataion(MeshData& meshData, Matrix4 matrix )
    {
        for( auto& vertex : meshData.vertices )
        {
            Vec4 tmp( vertex.x, vertex.y, vertex.z, 1);
            Vec4 tmp1 = matrix*tmp;
            vertex = Vec3( tmp1.x, tmp1.y, tmp1.z);
        }

        if( meshData.normals.empty() == false  )
        {
            Matrix4 normalMatrix=  glm::transpose( glm::inverse( matrix ));

            for( auto& normal : meshData.normals )
            {
                Vec4 tmp( normal.x, normal.y, normal.z, 0);
                Vec4 tmp1 = matrix*tmp;
                normal = glm::normalize(  Vec3( tmp1.x, tmp1.y, tmp1.z) );
            }
        }
    }

};

#endif //ADVANCEDGL_SHAPE_H
