#ifndef UNTANGLE_DEMO_H
#define UNTANGLE_DEMO_H

#include "idemo.h"
#include "aabb.h"
#include "UntangleDemoView.h"

class TriMeshRenderable;

class UntangleDemo : public IDemo
                    ,public IUntangleDemoModel
{
public:
	virtual ~UntangleDemo() {};

	bool init() override;
	bool update() override;
	bool destroy() override;

	void exeOneStep() override;
private:
	AABB m_box;
	std::vector<TriMeshRenderable*> m_meshes;
	Vector<MeshData> m_tessellationDatas;
	UntangleDemoView* m_pUI = nullptr;

};
#endif
