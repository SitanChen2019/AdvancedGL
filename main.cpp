//
// Created by Sitan Chen on 2019/8/28.
//
#include <iostream>
#include "global.h"

#include "Demos/ModelDisplay/ModelDisplay.h"
#include "Demos/CharacterAnimation/CharacterAnimation.h"
#include "Demos/TessellationShader/TessellationShader.h"
#include "Demos/Untangle/UntangleDemo.h"
#include "Demos/Triangulation/TrianglulationDemo.h"

void onGLFWErrorCallback( int error_id, const char* error_str)
{
    std::cout << error_str << std::endl;
}

 void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    App* app = (App*)glfwGetWindowUserPointer(window);
    if (app)
    {
        app->handleKeyInput(key, scancode, action, mods);
    }

}

 void resize_callback(GLFWwindow* window, int width, int height)
{
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (app)
    {
        app->resize( width, height);
    }

}

 void mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (app)
    {
        app->handleMouseInput(button, action, mods);
    }

}

 void mousemove_callback(GLFWwindow* window, double xpos, double ypos)
{
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (app)
    {
        app->handleMouseMove(xpos, ypos);
    }
}

 void mousescroll_callback(GLFWwindow* window, double x, double y)
{
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (app)
    {
        app->handleMouseScroll(x, y);
    }
}



void onGLFWWinddowResizeCallback( GLFWwindow* pWindow, int width , int height)
{
    void* pData =glfwGetWindowUserPointer(pWindow );
    RenderWindow* pRenderWindow = reinterpret_cast<RenderWindow*>(pData);
    if( pRenderWindow )
        pRenderWindow->onResizeWindow(width,height);
}

int main(int argc, const char** argv)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwSetErrorCallback( onGLFWErrorCallback);
    App& app = Global::s_app;
    GLFWwindow* window = app.createWindow();
    if( !window )
    {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, (void*)(&app));
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window,resize_callback );
    glfwSetMouseButtonCallback(window, mousebutton_callback);
    glfwSetCursorPosCallback(window, mousemove_callback);
    glfwSetScrollCallback(window, mousescroll_callback);

    if( !app.init() )
    {
        glfwTerminate();
        return -1;
    }

    int demoID = 5;
    IDemo* pDemo(nullptr);
    switch ( demoID )
    {
        case 1:
            pDemo = new ModelDisplay();
            break;
        case 2:
            pDemo = new CharacterAnimation();
            break;
        case 3:
            pDemo = new TessellationShader();
            break;
		case 4:
			pDemo = new UntangleDemo();
			break;
		case 5:
			pDemo = new TrianglulationDemo();
			break;
        default:
            assert(false);
    }

    app.run( pDemo );
    app.destroy();


    glfwTerminate();
    return 0;
}