//
// Created by Sitan Chen on 2019/9/7.
//

#ifndef ADVANCEDGL_CAMERA_CONTROL_H
#define ADVANCEDGL_CAMERA_CONTROL_H

#include "aabb.h"
#include "input_listener.h"
#include "render_listener.h"

class RenderWindow;

class CameraControl;


enum CameraControlType
{
    APEX_TYPE,
    UNKNOWN_TYPE,
};

class ICameraControlIpml {
public:
    virtual ~ICameraControlIpml(){};

    virtual CameraControlType getType() { return UNKNOWN_TYPE;}
    virtual EventHandleStatus handleMouseScroll(CameraControl* pCamera,float x, float y) { return EventUnHandled; };
    virtual EventHandleStatus handleMouseMove(CameraControl* pCamera,float xpos, float ypos) { return EventUnHandled; };
    virtual EventHandleStatus handleMouseInput(CameraControl* pCamera,int button, int action, int mods) { return EventUnHandled; };
    virtual EventHandleStatus handleKeyInput(CameraControl* pCamera,int button, int scancode, int action, int mods) { return EventUnHandled; };
};

class ApexControl : public ICameraControlIpml
{
    CameraControlType getType() override { return APEX_TYPE;}
    EventHandleStatus handleMouseScroll(CameraControl* pCamera,float x, float y) override;
    EventHandleStatus handleMouseMove(CameraControl* pCamera,float xpos, float ypos) override ;

private:
    void rotate(CameraControl* pCamera,float xpos, float ypos) ;
    void pan(CameraControl* pCamera,float xpos, float ypos);
};

class CameraControl : public  InputListener
                    , public  WindowEventListener {
public:
    void setCamearaControlType(CameraControlType type )
    {
        if( m_impl )
        {
            if( m_impl->getType() == type )
                return;
            delete( m_impl);
            m_impl = nullptr;
        }
        if( type == APEX_TYPE )
            m_impl = new ApexControl();

    }

    void bindRendingWindow( RenderWindow* pRenderingWindow)
    {
        m_pRenderWindow = pRenderingWindow;
    }

    void fitBox( const AABB&  box);

    EventHandleStatus  handleMouseScroll(float x, float y) override{
        return  m_impl == nullptr ? EventUnHandled : m_impl->handleMouseScroll(this, x,y);
    }

    EventHandleStatus handleMouseMove(float xpos, float ypos) override
    {
        return m_impl == nullptr ? EventUnHandled : m_impl->handleMouseMove(this ,xpos,ypos);
    }

    EventHandleStatus handleMouseInput(int button, int action, int mods) override
    {
        return m_impl == nullptr ? EventUnHandled : m_impl->handleMouseInput(this, button,action,mods);
    }

    EventHandleStatus handleKeyInput(int button, int scancode, int action, int mods) override
    {
        return m_impl == nullptr ? EventUnHandled : m_impl->handleKeyInput(this,button, scancode, action, mods);
    }

    void onRenderWindowResize( RenderWindow* pWindow, unsigned width, unsigned height ) override;

    float getFov() const{ return m_fov;}
    void setFov( float fov ) { m_fov = fov; }

    void updateProjMatrix();

    AABB getCurrentBox() const { return  m_currentBox;}
private:
    RenderWindow* m_pRenderWindow;
    ICameraControlIpml* m_impl = nullptr;

    float m_zear = 0.1f;
    float m_far = 1000.f;
    float m_fov = 60.0f;
    AABB m_currentBox;
};



#endif //ADVANCEDGL_CAMERA_CONTROL_H
