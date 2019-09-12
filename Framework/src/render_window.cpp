//
// Created by Sitan Chen on 2019/8/31.
//

#include "render_window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <global.h>

#include "render_listener.h"

void RenderWindow::configGLFW()
{
    auto glVersion =  Global::appSetting().getGLVersion();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersion.first);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersion.second);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   Global::appSetting().getGLFowardCompatEnable() ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   Global::appSetting().getGLCoreProfileEnable()? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
};

RenderWindow::RenderWindow( GLFWwindow* pGLFWwindow, unsigned width, unsigned height)
:m_pGLFWWindow(pGLFWwindow)
,m_winWidth(width)
,m_winHeight(height)
{

}

bool RenderWindow::init()
{
    return true;
}

void RenderWindow::destroy()
{
    for( auto it = m_Listeners.begin(); it != m_Listeners.end() ; ++it )
    {
        (*it)->onDetachRenderingWindow(this);
    }
    m_Listeners.clear();
}


GLFWwindow* RenderWindow::getGLFWWindow() {
    return m_pGLFWWindow;
}

void RenderWindow::onResizeWindow(int width, int height)
{
    m_winWidth = width;
    m_winHeight = height;
    fireWindowEvent();
    fireRenderEvent(RENDER_RESIZE, nullptr);

    glViewport(0,0, m_winWidth, m_winHeight);
}

void RenderWindow::registerRenderEventListener(RenderEventListener* pListener)
{
    if( std::find( m_Listeners.begin(), m_Listeners.end(), pListener) == m_Listeners.end())
    {
        m_Listeners.push_front( pListener );
        pListener->onAttachRenderingWindow( this );
    }
}

void RenderWindow::unregisterRenderEventListener(RenderEventListener* pListener)
{
    auto iter = std::find(  m_Listeners.begin(), m_Listeners.end(), pListener);
    if( iter != m_Listeners.end() )
    {
        pListener->onDetachRenderingWindow(this);
        m_Listeners.erase(iter);
    }
}

void RenderWindow::registerWindowEventListener(WindowEventListener* pListener)
{
    if( std::find( m_winListeners.begin(), m_winListeners.end(), pListener) == m_winListeners.end())
    {
        m_winListeners.push_front( pListener );
        pListener->onAttachRenderingWindow( this ,m_winWidth, m_winHeight );
    }
}

void RenderWindow::unregisterWindowEventListener(WindowEventListener* pListener)
{
    auto iter = std::find(  m_winListeners.begin(), m_winListeners.end(), pListener);
    if( iter != m_winListeners.end() )
    {
        pListener->onDetachRenderingWindow(this);
        m_winListeners.erase(iter);
    }
}

void RenderWindow::addRenderable( IRenderable* pRenderable )
{
    int queueID = pRenderable->getQueueID();
    auto  it = m_renderQueues.find(queueID);
    if( it == m_renderQueues.end() )
    {
        pRenderable->attach();
        m_renderQueues.insert( std::make_pair(queueID, RenderableList{pRenderable}));
        return;
    } else{
        pRenderable->attach();
        it->second.push_back( pRenderable );
    }
}

void RenderWindow::removeRenderable( IRenderable* pRenderable)
{
    int queueID = pRenderable->getQueueID();
    auto  it = m_renderQueues.find(queueID);
    if( it != m_renderQueues.end() )
    {
        auto it1 = std::find( it->second.begin(), it->second.end(), pRenderable );
        if( it1 != it->second.end() )
        {
            pRenderable->detach();
            it->second.erase( it1 );
        }
    }
}

void RenderWindow::update()
{
    fireRenderEvent(FRAME_BEGIN_EVENT, nullptr);
    glClearColor(0,0,0,1);
    glClearDepth(1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    for( auto it = m_renderQueues.begin(); it != m_renderQueues.end(); ++it)
    {
        int queueID = it->first;
        fireRenderEvent(RENDER_QUEUE_START_EVENT, (void*)&queueID);

        for( auto it1 = it->second.begin(); it1 != it->second.end(); ++it1 )
        {
            (*it1)->render();
        }

        fireRenderEvent(RENDER_QUEUE_END_EVENT, (void*)&queueID);
    }

    glfwSwapBuffers( m_pGLFWWindow);
}

void RenderWindow::fireRenderEvent(RenderEventIDS eventType, void *data)
{
    for( auto it = m_Listeners.rbegin(); it != m_Listeners.rend(); ++it )
    {
        RenderEventListener* pListener = *it;
        if (eventType == RENDER_RESIZE )
        {
            pListener->onRenderWindowResize(this, m_winWidth, m_winHeight);
        }
        else if( eventType == RenderEventIDS::RENDER_QUEUE_START_EVENT)
        {
            int* pQueueID = reinterpret_cast<int*>(data);
            pListener->onRenderQueueStart(this, *pQueueID);
        }
        else if( eventType == RenderEventIDS::RENDER_QUEUE_END_EVENT)
        {
            int* pQueueID = reinterpret_cast<int*>(data);
            pListener->onRenderQueueEnd(this, *pQueueID);
        }
        else if( eventType == RenderEventIDS::FRAME_BEGIN_EVENT)
        {
            pListener->onFrameBegin(this);
        }
        else if( eventType == RenderEventIDS::FRAME_END_EVENT )
        {
            pListener->onFrameEnd(this);
        }
    }
}

void RenderWindow::fireWindowEvent()
{
    for( auto it = m_winListeners.rbegin(); it != m_winListeners.rend(); ++it ) {
        WindowEventListener* pListener = *it;
        pListener->onRenderWindowResize(this, m_winWidth, m_winHeight);
    }
}