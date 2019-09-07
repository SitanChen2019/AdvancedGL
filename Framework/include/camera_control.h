//
// Created by Sitan Chen on 2019/9/7.
//

#ifndef ADVANCEDGL_CAMERA_CONTROL_H
#define ADVANCEDGL_CAMERA_CONTROL_H

#include "aabb.h"

class RenderWindow;

class CameraControl {
public:
    void bindRendingWindow( RenderWindow* pRenderingWindow)
    {
        m_pRenderWindow = pRenderingWindow;
    }

    void fitBox( const AABB&  box);
    void setProjectMatrix();


private:
    RenderWindow* m_pRenderWindow;
};


#endif //ADVANCEDGL_CAMERA_CONTROL_H
