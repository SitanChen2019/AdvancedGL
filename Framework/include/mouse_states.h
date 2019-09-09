//
// Created by Sitan Chen on 2019/9/9.
//

#ifndef ADVANCEDGL_MOUSE_STATES_H
#define ADVANCEDGL_MOUSE_STATES_H

#include "boost/circular_buffer.hpp"
#include "GLFW/glfw3.h"

struct MousePosition
{
    double x;
    double y;
};

class MouseStatus
{
public:
    bool isLeftDown() { return m_bLeftDown && !m_bMiddleDown && !m_bRightDown; }
    bool isRightDown() { return !m_bLeftDown && !m_bMiddleDown && m_bRightDown; }
    bool isMiddleDown() { return !m_bLeftDown && m_bMiddleDown && !m_bRightDown; }

    bool isLeftRightDown() { return m_bLeftDown && m_bRightDown && !m_bMiddleDown; }

    void onMouseButtonPress( int  btn)
    {
        switch (btn)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_bLeftDown = true;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                m_bRightDown = true;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                m_bMiddleDown = true;
                break;
            default:
                break;
        }
    }



    void onMouseButtonRelease( int  btn)
    {
        switch (btn)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_bLeftDown = false;
                break;
            case  GLFW_MOUSE_BUTTON_RIGHT:
                m_bRightDown = false;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                m_bMiddleDown = false;
                break;
            default:
                break;
        }
    }

    void setPosition(double x, double y)
    {
        m_mousePosQueue.push_back(MousePosition{ x,y });
    }


    MousePosition getLastDeltaPosition()
    {
        auto it1 = m_mousePosQueue.rbegin();
        auto it2 = it1+1;
        if (it1 != m_mousePosQueue.rend() && it2 != m_mousePosQueue.rend())
        {
            auto ret = MousePosition{ it1->x - it2->x,
                                      it1->y - it2->y };
            return ret;
        }
        return MousePosition{ 0,0 };
    }

private:
    bool m_bLeftDown = false;
    bool m_bRightDown = false;
    bool m_bMiddleDown = false;

    boost::circular_buffer<MousePosition> m_mousePosQueue = boost::circular_buffer<MousePosition>(2);
};



#endif //ADVANCEDGL_MOUSE_STATES_H
