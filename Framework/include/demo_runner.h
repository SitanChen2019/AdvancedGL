//
// Created by Sitan Chen on 2019/9/2.
//

#ifndef ADVANCEDGL_DEMO_RUNNER_H
#define ADVANCEDGL_DEMO_RUNNER_H

class IDemo;

class DemoRunner
{
public:
    void unloadCurrent();
    void loadDemo( IDemo* pDemo );

    void update();

private:
    IDemo* m_pCurrentDemo = nullptr;
};
#endif //ADVANCEDGL_DEMO_RUNNER_H
