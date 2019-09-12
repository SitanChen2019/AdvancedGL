//
// Created by sitan on 19-9-12.
//

#ifndef ADVANCEDGL_MODELDISPLAYUIVIEW_H
#define ADVANCEDGL_MODELDISPLAYUIVIEW_H

#include "iui_view.h"



class IModelDIsplayUIModel
{
public:
    virtual void displayAsPureColor() = 0;
    virtual void displayAsHardLighting() = 0;
    virtual void displayAsSmoothLighting() = 0;
    virtual void reloadShader() = 0;
};


class ModelDisplayUIView : public IUIView {

public:
    ModelDisplayUIView( IModelDIsplayUIModel* pModel)
    :m_pModel( pModel ) {}

    void drawUI();

private:
    IModelDIsplayUIModel* const m_pModel;

};


#endif //ADVANCEDGL_MODELDISPLAYUIVIEW_H
