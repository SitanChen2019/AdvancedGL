//
// Created by Sitan Chen on 2019/9/3.
//

#include "app.h"
#include "global.h"

GLFWwindow* App::createWindow()
{
    AppSetting& setting = AppSetting::singletion();

    RenderWindow::configGLFW();

    /* Create a windowed mode window and its OpenGL context */
    m_pWindow = glfwCreateWindow(setting.getWindowInitWidth(),
                              setting.getWindowInitHeiht(),
                              setting.getWindowTitle().c_str(),
                              NULL, NULL);

    return m_pWindow;
}

bool App::init()
{
    AppSetting& setting = AppSetting::singletion();
    m_pRenderingWindow = new RenderWindow(m_pWindow, setting.getWindowInitWidth(),
                                                   setting.getWindowInitHeiht());


    m_cameraControl.bindRendingWindow( m_pRenderingWindow );
    m_cameraControl.setCamearaControlType(APEX_TYPE);
    m_pRenderingWindow->registerWindowEventListener(&m_cameraControl);

    m_inputHandles.push_back(&m_cameraControl);

    /* Make the window's context current */
    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL();

    if( !m_pRenderingWindow->init())
        return false;

    m_shaderMgr.init();
    m_pRenderingWindow->registerRenderEventListener( &Global::uiManager() );
    return true;
}

void App::run( IDemo* pDemo )
{
    DemoRunner runner;

    runner.loadDemo( pDemo );
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(m_pWindow))
    {
        /* Poll for and process events */
        glfwPollEvents();

        runner.update();

        m_pRenderingWindow->update();
    }

    runner.unloadCurrent();
}

void App::destroy()
{
    if( m_pRenderingWindow)
    {
        m_pRenderingWindow->destroy();
        delete m_pRenderingWindow;
        m_pRenderingWindow = nullptr;
    }

    m_inputHandles.clear();
    glfwDestroyWindow( m_pWindow );
    m_pWindow = nullptr;

}

void App::resize(int w, int h)
{
    if( m_pRenderingWindow )
        m_pRenderingWindow->onResizeWindow(w,h);
}

void App::handleKeyInput(int button, int scancode, int action, int mods)
{
    for( auto handle : m_inputHandles )
        if( handle->handleKeyInput(button,scancode,action,mods) == EventHandled )
            return;
}

void App::handleMouseInput(int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        m_mouseStatus.onMouseButtonPress(button);
    }
    else if (action == GLFW_RELEASE)
    {
        m_mouseStatus.onMouseButtonRelease(button);
    }

    for( auto handle : m_inputHandles )
        if( handle->handleMouseInput(button,action,mods) == EventHandled )
            return;
}

void App::handleMouseMove(double xpos, double ypos)
{
    m_mouseStatus.setPosition(xpos,ypos);

    for( auto handle : m_inputHandles )
        if( handle->handleMouseMove( float(xpos), float(ypos)) == EventHandled )
            return;
}

void App::handleMouseScroll(double x, double y)
{
    for( auto handle : m_inputHandles )
        if( handle->handleMouseScroll((float)x,(float)y) == EventHandled )
            return;
}