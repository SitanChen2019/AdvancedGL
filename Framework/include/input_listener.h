//
// Created by Sitan Chen on 2019/9/8.
//

#ifndef ADVANCEDGL_INPUT_LISTENER_H
#define ADVANCEDGL_INPUT_LISTENER_H

enum EventHandleStatus
{
    EventUnHandled,
    EventHandled
};

class InputListener
{
public:
    virtual ~InputListener(){};

    virtual EventHandleStatus handleKeyInput(int button, int scancode, int action, int mods)
    {
        return EventUnHandled;
    }

    virtual  EventHandleStatus handleMouseInput(int button, int action, int mods)
    {
        return EventUnHandled;
    }

    virtual  EventHandleStatus handleMouseMove(float xpos, float ypos)
    {
        return EventUnHandled;
    }

    virtual EventHandleStatus  handleMouseScroll(float x, float y)
    {
        return EventUnHandled;
    }
};

#endif //ADVANCEDGL_INPUT_LISTENER_H
