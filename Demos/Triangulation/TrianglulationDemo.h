#ifndef TRIANGLULATION_DEMO_H
#define TRIANGLULATION_DEMO_H

#include "idemo.h"
#include "aabb.h"
#include "triangle.h"
#include "TriangulationRenderables.h"
#include "TriangulationDemoUI.h"

class TrianglulationDemo : public IDemo, public ITriangulationUIModel
{
public:
	virtual ~TrianglulationDemo() {};

	bool init() override;
	bool update() override;
	bool destroy() override;

	void displayPreModel() override;
	void displayPostModel() override;
	void displayRefineModel() override;


	void quicktest();

	void createMeshes(const triangulateio& data, std::set< IRenderable*>& meshGroup);
	void removeMeshGroup(std::set<IRenderable*>& meshGroup);
	void showMeshGroup(const std::set<IRenderable*>& meshGroup, bool enable);
private:
	void fillInputData1(triangulateio& input_data);
	
private:
	AABB m_box;
	std::set<IRenderable*> m_preMeshes;
	std::set<IRenderable*> m_postMeshes;
	std::set<IRenderable*> m_refineMesh;
	TriangulationUIView* m_pUI = nullptr;
};
#endif
