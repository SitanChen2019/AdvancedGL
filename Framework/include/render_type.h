//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_RENDER_TYPE_H
#define ADVANCEDGL_RENDER_TYPE_H

#include <vector>
#include "glm/glm.hpp"
#include "cstsequence.h"



template<class T>
using Vector = std::vector<T>;

using Vec2 = glm::tvec2<float>;
using Vec3 = glm::tvec3<float>;
using Vec4 = glm::tvec4<float>;
using Matrix4 = glm::mat4x4;
using Vec2Sequence = Vector<Vec2>;
using Vec3Sequence = Vector<Vec3>;
using Int32Sequence = Vector<unsigned>;



enum PrimitiveType
{
    PT_POINT,
    PT_LINES,
    PT_TRIANGLE,
};

struct MeshData
{
    PrimitiveType  primType;
    Vec3Sequence vertices;
    Int32Sequence indices;
    Vec3Sequence  normals;
    Vec2Sequence textCoors;
};

const Vec3 COLOR_WHITE = Vec3(1,1,1);

#endif //ADVANCEDGL_RENDER_TYPE_H
