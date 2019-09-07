//
// Created by Sitan Chen on 2019/9/7.
//

#ifndef ADVANCEDGL_UTILITIES_H
#define ADVANCEDGL_UTILITIES_H

#include "render_type.h"
#include <sstream>

class Utilities {

public:
    static std::string str(const Vec3 vec )
    {
        std::stringstream ss;
        ss << "[" << vec.x << " " << vec.y << " " << vec.z<<"]";
        return ss.str();
    }

    static std::string str(const Matrix4& matrix )
    {
        std::stringstream ss;
        ss << "[ (" << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3] <<"),";
        ss << "(" << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3] <<"),";
        ss << "(" << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3] <<"),";
        ss << "(" << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[3][3] <<")]";
        return ss.str();
    }

};


#endif //ADVANCEDGL_UTILITIES_H
