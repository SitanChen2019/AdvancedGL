//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_UIMANAGER_H
#define ADVANCEDGL_UIMANAGER_H

#include <list>

#include "render_listener.h"
#include "irenderable.h"
#include "iui_view.h"

struct ImFont;

class UIManager : public  RenderEventListener {

public:
    void onAttachRenderingWindow( RenderWindow* pWindow ) override ;
    void onRenderQueueStart(RenderWindow* pWindow, int pQueueID) override ;
    void onRenderQueueEnd(RenderWindow* pWindow, int pQueueID) override;
    void onDetachRenderingWindow(RenderWindow* pWindow) override ;

    void addUIView(IUIView* pView);
    void removeUIView(IUIView* pView);

private:
    void drawUI();

private:
    DummyRenderable m_queueStart = DummyRenderable(0);
    DummyRenderable m_queueEnd = DummyRenderable(255);

    std::list<IUIView*> m_UIViews;
    ImFont*                mFont;
};


#endif //ADVANCEDGL_UIMANAGER_H
