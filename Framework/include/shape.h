//
// Created by Sitan Chen on 2019/9/7.
//

#ifndef ADVANCEDGL_SHAPE_H
#define ADVANCEDGL_SHAPE_H

#include "render_type.h"

class Shape
{
public:
    static MeshData createTestTriangle()
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
};

#endif //ADVANCEDGL_SHAPE_H
