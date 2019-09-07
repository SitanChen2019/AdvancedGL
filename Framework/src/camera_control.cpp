//
// Created by Sitan Chen on 2019/9/7.
//

#include "camera_control.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "render_window.h"


void CameraControl::fitBox( const AABB&  box)
{
    Vec3 diagonal_vec = box.maxP() - box.minP();
    float area_xy = fabsf( diagonal_vec.x * diagonal_vec.y );
    float area_yz = fabsf( diagonal_vec.y * diagonal_vec.z );
    float area_xz = fabsf( diagonal_vec.z * diagonal_vec.x );

    Vec3 lookDir;
    Vec3 up;
    if( area_yz > area_xy && area_yz > area_xz )
    {
        lookDir =  Vec3(1,0,0);
        up = Vec3(0,1,0);
    }
    else if( area_xz > area_xy && area_xz > area_yz )
    {
        lookDir =  Vec3(0,1,0);
        up = Vec3(0,0,-1 );
    }
    else
    {
        lookDir =  Vec3(0,0,1);
        up = Vec3(0,1,0);
    }


    float diagonal = box.diagonalLength();
    float r = diagonal == 0 ? 1 : diagonal;

    float fov = glm::radians(60.0f);
    unsigned  width =  m_pRenderWindow->getWinWidth();
    unsigned height = m_pRenderWindow->getWinHeight();

    float znear = r;
    float zfar = 3*r;
    auto projectMatrix = glm::perspectiveFov(fov, (float)width, (float)height, znear, zfar);
    m_pRenderWindow->setProjMatrix( projectMatrix );


    Vec3 pos = box.center() - 2*r*lookDir;

    auto matView = glm::lookAt( pos, box.center(), up);
    m_pRenderWindow->setViewMatrix( matView );
}

void CameraControl::setProjectMatrix()
{

}