//
// Created by Sitan Chen on 2019/9/2.
//

#include "demo_runner.h"
#include "idemo.h"

void DemoRunner::update() {
    if( m_pCurrentDemo )
        m_pCurrentDemo->update();
}

void DemoRunner::loadDemo(IDemo *pDemo) {
    if( m_pCurrentDemo )
        return;

    m_pCurrentDemo = pDemo;
    m_pCurrentDemo->init();
}

void DemoRunner::unloadCurrent() {
    if( m_pCurrentDemo )
    {
        m_pCurrentDemo->destroy();
        delete m_pCurrentDemo;
        m_pCurrentDemo = nullptr;
    }
}