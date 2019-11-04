//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_APP_H
#define ADVANCEDGL_APP_H

#include <list>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "render_window.h"
#include "demo_runner.h"
#include "app_setting.h"
#include "ui_manager.h"
#include "globject.h"
#include "shader.h"
#include "camera_control.h"
#include "input_listener.h"
#include "mouse_states.h"
#include "mouse_picking.h"


class Global;
class IDemo;

class App {
public:
    GLFWwindow* createWindow();
    bool init();
    void run(IDemo* pDemo);
    void destroy();

    void resize(int w, int h);
    void handleKeyInput(int button, int scancode, int action, int mods);
    void handleMouseInput(int button, int action, int mods);
    void handleMouseMove(double xpos, double ypos);
    void handleMouseScroll(double x, double y);

private:
    GLFWwindow* m_pWindow = nullptr ;
    RenderWindow* m_pRenderingWindow = nullptr;

    AppSetting m_setting;
    UIManager m_uiManager;
    GLObjectMgr m_glObjMgr;
    ShaderMgr m_shaderMgr;
    CameraControl m_cameraControl;
    MouseStatus m_mouseStatus;
    MousePicking m_mousePicking;

    std::list<InputListener*> m_inputHandles;

    friend class Global;
};





#endif //ADVANCEDGL_APP_H
