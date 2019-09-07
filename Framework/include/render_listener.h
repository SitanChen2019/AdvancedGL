//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_I_RENDER_EVENT_LISTENER_H
#define ADVANCEDGL_I_RENDER_EVENT_LISTENER_H


#include "render_window.h"

struct GLFWwindow;

class RenderEventListener
{
public:
    virtual void onAttachRenderingWindow( RenderWindow* pWindow ) {};

    virtual void onRenderQueueStart(RenderWindow* pWindow, int pQueueID) {};
    virtual void onRenderQueueEnd(RenderWindow* pWindow, int pQueueID) {};

    virtual void onRenderWindowResize( RenderWindow* pWindow, unsigned width, unsigned height ){};

    virtual void onFrameBegin(RenderWindow* pWindow){};
    virtual void onFrameEnd(RenderWindow* pWindow){};

    virtual void onDetachRenderingWindow(RenderWindow* pWindow){};
};

class WindowEventListener
{
public:
    virtual void onAttachRenderingWindow( RenderWindow* pWindow , unsigned width, unsigned height) {};
    virtual void onRenderWindowResize( RenderWindow* pWindow, unsigned width, unsigned height ){};
    virtual void onDetachRenderingWindow(RenderWindow* pWindow){};
};


#endif //ADVANCEDGL_I_RENDER_EVENT_LISTENER_H
