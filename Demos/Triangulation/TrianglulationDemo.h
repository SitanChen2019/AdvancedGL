#ifndef TRIANGLULATION_DEMO_H
#define TRIANGLULATION_DEMO_H

#include "idemo.h"
#include "aabb.h"
#include "triangle.h"
#include "TriangulationRenderables.h"


class TrianglulationDemo : public IDemo

{
public:
	virtual ~TrianglulationDemo() {};

	bool init() override;
	bool update() override;
	bool destroy() override;


	void quicktest();

	void createMeshes(const triangulateio& data, std::set< IRenderable*>& meshGroup);
	void removeMeshGroup(std::set<IRenderable*>& meshGroup);
private:
	std::string fillInputData1(triangulateio& input_data);
	
private:
	AABB m_box;
	std::set<IRenderable*> m_preMeshes;
	std::set<IRenderable*> m_postMeshes;


};
#endif
