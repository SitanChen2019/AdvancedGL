//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_APP_SETTING_H
#define ADVANCEDGL_APP_SETTING_H

#include <string>

class AppSetting
{
public:
    static AppSetting& singletion()
    {
        static AppSetting s_setting;
        return s_setting;
    }

    //Windows Setting
    unsigned  getWindowInitWidth()
    {
        return 1000;
    }

    unsigned  getWindowInitHeiht()
    {
        return 800;
    }

    std::string getWindowTitle()
    {
        return "Advanced GL";
    }

    //OpenGL Setting
    std::pair<unsigned,unsigned>  getGLVersion()
    {
        return std::make_pair(3,3);
    }

    std::string getGLSLVersion()
    {
        return "#version 330";
    }

    bool  getGLFowardCompatEnable()
    {
        //if ture, all the functions/features marked as "deprecated" will be not available.
        return true;
    }

    bool getGLCoreProfileEnable()
    {
        //if ture, all the functions removed from core porfiles will be not available.
        return true;
    }
};
#endif //ADVANCEDGL_APP_SETTING_H
