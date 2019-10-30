//
// Created by Sitan Chen on 2019/9/7.
//
#include <iostream>

#include "camera_control.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "render_window.h"
#include "global.h"

void CameraControl::fitBox( const AABB&  box)
{
    m_currentBox = box;

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
        lookDir =  Vec3(0,0,-1);
        up = Vec3(0,-1,0);
    }


    float diagonal = box.diagonalLength();
    float r = diagonal == 0 ? 1 : diagonal;

    m_fov = 60.0f;
    m_zear = r;
    m_far = 3*r;
    updateProjMatrix();

    Vec3 pos = box.center() - 2*r*lookDir;

    auto matView = glm::lookAt( pos, box.center(), up);
    m_pRenderWindow->setViewMatrix( matView );
}

void CameraControl::onRenderWindowResize(RenderWindow* pWindow, unsigned width, unsigned height )
{
    updateProjMatrix();
}

void CameraControl::updateProjMatrix()
{
    float fov = glm::radians(m_fov);
    unsigned  width =  m_pRenderWindow->getWinWidth();
    unsigned height = m_pRenderWindow->getWinHeight();
    auto projectMatrix = glm::perspectiveFov(fov, (float)width, (float)height, m_zear, m_far);
    m_pRenderWindow->setProjMatrix( projectMatrix );
}


EventHandleStatus ApexControl::handleMouseScroll(CameraControl* pCamera,float x, float y)
{
    float fov =  pCamera->getFov();
    if( y < 0 )
        fov *= 0.95f;
    else
        fov /= 0.95f;

    if( fov < 0.1f )
        fov = 0.1f;
    if( fov > 179.f )
        fov = 179.f;

    pCamera->setFov( fov );
    pCamera->updateProjMatrix();
    return EventHandled;
};
void ApexControl::rotate(CameraControl* pCamera,float deltaX, float deltaY)
{
    auto center3 = pCamera->getCurrentBox().center();
    auto currentViewMat =  Global::renderWindow().getViewMatrix();
    auto currentViewMatInverse = glm::inverse(currentViewMat);

    if( abs(deltaX) > 1 )
    {
        float angle = 2*deltaX*360.0f/Global::renderWindow().getWinWidth();

        auto up = currentViewMatInverse*Vec4(0,1,0,0);

        auto identifyMat = glm::identity<Matrix4>();
        auto centerToOrigin = glm::translate(identifyMat, -center3);

        Vec3 up3 = Vec3(up.x,up.y,up.z);
        auto rotMat = glm::rotate( identifyMat, glm::radians(angle), up3);

        auto originToCenter = glm::translate(identifyMat, center3);

        auto finalMat = currentViewMat*originToCenter* rotMat *centerToOrigin ;

        Global::renderWindow().setViewMatrix( finalMat );
    }

    //else
    if( fabs(deltaY) > 1)
    {
        float angle = 2*deltaY*360.0f/Global::renderWindow().getWinHeight();
        auto currentViewMat =  Global::renderWindow().getViewMatrix();
        auto right = currentViewMatInverse*Vec4(1,0,0,0);


        auto identifyMat = glm::identity<Matrix4>();
        auto centerToOrigin = glm::translate(identifyMat, -center3);

        Vec3 right3 = Vec3(right.x,right.y,right.z);
        auto rotMat = glm::rotate( identifyMat, glm::radians( angle) , right3);

        auto originToCenter = glm::translate(identifyMat, center3);

        auto finalMat =  currentViewMat*originToCenter* rotMat* centerToOrigin ;

        Global::renderWindow().setViewMatrix( finalMat );
    }
}

void ApexControl::pan(CameraControl* pCamera,float deltaX, float deltaY)
{
    float offsetX = 2*deltaX/Global::renderWindow().getWinWidth();
    float offsetY = 2*deltaY/Global::renderWindow().getWinHeight();
    auto identifyMatrix = glm::identity<Matrix4>();
    auto offsetMatrix = glm::translate( identifyMatrix, Vec3(offsetX,-offsetY,0) );
    auto finalMat = offsetMatrix * Global::renderWindow().getProjMatrix();
    Global::renderWindow().setProjMatrix( finalMat );

}


EventHandleStatus ApexControl::handleMouseMove(CameraControl* pCamera,float xpos, float ypos)
{
    if(  Global::mouseState().isLeftDown())
    {
        auto mouseDelta = Global::mouseState().getLastDeltaPosition();
        rotate( pCamera, (float)mouseDelta.x, (float)mouseDelta.y);
        return  EventHandled;
    }
    else if( Global::mouseState().isRightDown() )
    {
        auto mouseDelta = Global::mouseState().getLastDeltaPosition();
        pan( pCamera, (float)mouseDelta.x, (float)mouseDelta.y);
        return  EventHandled;
    }


    return  EventUnHandled;
}

