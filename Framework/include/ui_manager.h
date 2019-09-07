//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_UIMANAGER_H
#define ADVANCEDGL_UIMANAGER_H

#include "render_listener.h"
#include "irenderable.h"

class UIManager : public  RenderEventListener {

    void onAttachRenderingWindow( RenderWindow* pWindow ) override ;
    void onRenderQueueStart(RenderWindow* pWindow, int pQueueID) override ;
    void onRenderQueueEnd(RenderWindow* pWindow, int pQueueID) override;
    void onDetachRenderingWindow(RenderWindow* pWindow) override ;

private:
    DummyRenderable m_queueStart = DummyRenderable(0);
    DummyRenderable m_queueEnd = DummyRenderable(255);
};


#endif //ADVANCEDGL_UIMANAGER_H
