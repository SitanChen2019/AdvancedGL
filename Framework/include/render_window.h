//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_RENDERWINDOW_H
#define ADVANCEDGL_RENDERWINDOW_H

#include <list>
#include <map>
#include <vector>
#include <set>
#include "render_type.h"
#include "aabb.h"

class RenderEventListener;
class WindowEventListener;
struct GLFWwindow;
class IRenderable;

enum EventDomain{
    RENDER_EVENTS,
    WINDOW_EVENTS
};
enum RenderEventIDS
{
    RENDER_RESIZE,
    RENDER_QUEUE_START_EVENT,
    RENDER_QUEUE_END_EVENT,
    FRAME_BEGIN_EVENT,
    FRAME_END_EVENT
};

class RenderWindow
{
public:
    static void configGLFW();

    RenderWindow( GLFWwindow* pGLFWwindow, unsigned width, unsigned height);
    bool init();
    void destroy();

    GLFWwindow* getGLFWWindow();

    void onResizeWindow(int width , int height);
    void update();

    void registerRenderEventListener(RenderEventListener* pListener);
    void unregisterRenderEventListener(RenderEventListener* pListener);

    void registerWindowEventListener(WindowEventListener* pListener);
    void unregisterWindowEventListener(WindowEventListener* pListener);

    void fireRenderEvent( RenderEventIDS eventType, void* data);
    void fireWindowEvent();

    void addRenderable( IRenderable* pRenderable );
    void removeRenderable( IRenderable* pRenderable);

    const Matrix4& getProjMatrix() const { return m_projMatrix; }
    const Matrix4& getViewMatrix() const { return m_viewMatrix; }
    const Matrix4 getProjViewMatrix() const {return m_projMatrix*m_viewMatrix;}

    void setProjMatrix(const Matrix4& mat) { m_projMatrix = mat;}
    void setViewMatrix(const Matrix4& mat) { m_viewMatrix = mat;}

    unsigned getWinHeight() { return m_winHeight; }
    unsigned getWinWidth() {return m_winWidth; }
private:
    GLFWwindow* m_pGLFWWindow;
    unsigned    m_winWidth;
    unsigned    m_winHeight;

    std::list<RenderEventListener*> m_Listeners;
    std::list<WindowEventListener*> m_winListeners;

    using RenderableList = std::list<IRenderable*> ;
    std::map<int, RenderableList> m_renderQueues;

    Matrix4 m_viewMatrix;
    Matrix4 m_projMatrix;

};
#endif //ADVANCEDGL_RENDERWINDOW_H
