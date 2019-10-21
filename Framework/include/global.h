//
// Created by Sitan Chen on 2019/8/31.
//

#ifndef ADVANCEDGL_GLOBAL_H
#define ADVANCEDGL_GLOBAL_H

#include "app.h"

class Global
{
public:
    static App s_app;

    static AppSetting& appSetting(){
        return s_app.m_setting;
    }

    static UIManager& uiManager() {
        return s_app.m_uiManager;
    }

    static GLObjectMgr& glObjMgr() {
        return s_app.m_glObjMgr;
    }

    static ShaderMgr& shaderMgr() {
        return s_app.m_shaderMgr;
    }

    static RenderWindow& renderWindow() {
        return *s_app.m_pRenderingWindow;
    }

    static CameraControl& cameraControl() {
        return s_app.m_cameraControl;
    }

    static MouseStatus& mouseState() {
        return s_app.m_mouseStatus;
    }
    
    static const std::string SystemResPath( std::string subPath )
    {
#ifdef _MSC_BUILD
		std::string path = "../res";
#else
		std::string path = "/Users/Sitan/git/AdvancedGL/res";
#endif
        
        
        return path + subPath;
    }
    
    static const std::string DemoPath( std::string subPath )
    {
#ifdef _MSC_BUILD
		std::string path = "../Demos";
#else
		std::string path = "/Users/Sitan/git/AdvancedGL/Demos";
#endif
        return path + subPath;
    }
};
#endif //ADVANCEDGL_GLOBAL_H
