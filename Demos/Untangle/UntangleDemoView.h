#ifndef UNTANGLE_DEMO_VIEW_H
#define UNTANGLE_DEMO_VIEW_H
#include "iui_view.h"
#include <string>

class IUntangleDemoModel
{
public:
    virtual void exeOneStep() = 0;

	virtual void reloadModel(std::string modelName) = 0;
	virtual bool isGlobalScheme() = 0;
	virtual void setGlobalScheme(bool bValue) = 0;

	virtual int getMeshCount() = 0;
	virtual void setMeshInvMass(int meshID, float value) = 0;
	virtual float getMeshInvMass(int meshID) = 0;
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