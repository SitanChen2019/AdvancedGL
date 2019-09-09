//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_IDEMO_H
#define ADVANCEDGL_IDEMO_H

#include "render_window.h"
#
class IDemo
{
public:
    virtual ~IDemo(){}
    virtual bool init() = 0;
    virtual bool update() = 0;
    virtual bool destroy()= 0;
};
#endif //ADVANCEDGL_IDEMO_H
