//
// Created by Sitan Chen on 2019/8/31.
//

#include "ui_manager.h"
#include "render_window.h"
#include "global.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "irenderable.h"

void  UIManager::onAttachRenderingWindow( RenderWindow* pWindow )
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;


    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(pWindow->getGLFWWindow(), true);


    const char* glslVersion = Global::appSetting().getGLSLVersion().c_str();
    ImGui_ImplOpenGL3_Init(glslVersion);

    pWindow->addRenderable( &m_queueStart );
    pWindow->addRenderable( &m_queueEnd );
}

void  UIManager::onRenderQueueStart(RenderWindow* pWindow, int pQueueID)
{
    if( pQueueID == m_queueStart.getQueueID() )
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
}

void  UIManager::onRenderQueueEnd(RenderWindow* pWindow, int pQueueID)
{
    if( pQueueID == m_queueEnd.getQueueID() )
    {
        ImGui::Begin("HelloWorld");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void  UIManager::onDetachRenderingWindow(RenderWindow* pWindow)
{
    pWindow->removeRenderable(&m_queueStart);
    pWindow->removeRenderable(&m_queueEnd);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

