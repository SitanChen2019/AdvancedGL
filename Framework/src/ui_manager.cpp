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

    float size_pixels = 20;
    mFont = io.Fonts->AddFontFromFileTTF(Global::SystemResPath("/font/PINGFANG BOLD.TTF").c_str(), size_pixels);
}

void  UIManager::onRenderQueueStart(RenderWindow* pWindow, int pQueueID)
{
    if( pQueueID == m_queueStart.getQueueID() )
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawUI();
    }
}

void  UIManager::onRenderQueueEnd(RenderWindow* pWindow, int pQueueID)
{
    if( pQueueID == m_queueEnd.getQueueID() )
    {


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

void UIManager::addUIView(IUIView *pView) {

    std::list<IUIView*>::iterator it = m_UIViews.begin();
    for( ; it != m_UIViews.end(); ++it )
    {
        if( *it == pView )
            break;
    }

    if( it == m_UIViews.end() )
        m_UIViews.push_back(pView);
}

void UIManager::removeUIView(IUIView* pView)
{
        std::list<IUIView*>::iterator it = m_UIViews.begin();
        for( ; it != m_UIViews.end(); ++it )
        {
            if( *it == pView )
                break;
        }

        if( it != m_UIViews.end() )
        m_UIViews.erase(it);
}


void UIManager::drawUI() {
    ImGui::PushFont(mFont);
    for( std::list<IUIView*>::iterator it = m_UIViews.begin();it != m_UIViews.end(); ++it )
    {
        (*it)->drawUI();
    }
    ImGui::PopFont();

}
