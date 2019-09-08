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

    /* Make the window's context current */
    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL();

    if( !m_pRenderingWindow->init())
        return false;

    m_shaderMgr.init();
    //m_pRenderingWindow->registerRenderEventListener( &Global::uiManager() );
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

}

void App::handleMouseInput(int button, int action, int mods)
{

}

void App::handleMouseMove(double xpos, double ypos)
{

}

void App::handleMouseScroll(double x, double y)
{

}