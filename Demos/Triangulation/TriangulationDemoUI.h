//
// Created by sitan on 19-9-12.
//

#ifndef ADVANCEDGL_TRIANGULATION_DEMO_UIVIEW_H
#define ADVANCEDGL_TRIANGULATION_DEMO_UIVIEW_H

#include "iui_view.h"



class ITriangulationUIModel
{
public:
	virtual void displayPreModel() = 0;
	virtual void displayPostModel() = 0;
	virtual void displayRefineModel() = 0;
};


class TriangulationUIView : public IUIView {

public:
	TriangulationUIView(ITriangulationUIModel* pModel)
		:m_pModel(pModel) {}

	void drawUI();

private:
	ITriangulationUIModel* const m_pModel;

};


#endif //ADVANCEDGL_MODELDISPLAYUIVIEW_H
