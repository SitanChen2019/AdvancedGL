#ifndef UNTANGLE_DEMO_VIEW_H
#define UNTANGLE_DEMO_VIEW_H
#include "iui_view.h"

class IUntangleDemoModel
{
public:
    virtual void exeOneStep() = 0;
};

class UntangleDemoView : public IUIView
{
public:
    UntangleDemoView( IUntangleDemoModel* pModel)
    :m_pModel( pModel ) {}

    void drawUI() override ;

private:
    IUntangleDemoModel* m_pModel;
};
#endif