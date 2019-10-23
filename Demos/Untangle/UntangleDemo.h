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
	void reloadModel(std::string modelName) override;

	 bool isGlobalScheme() override;
	 void setGlobalScheme(bool bValue) override;

private:
	void optimizeMesh( MeshData& );
	void loadModel(std::string modelFullPath );

	 int getMeshCount() override;
	 void setMeshInvMass(int meshID, float value) override;
	 float getMeshInvMass(int meshID) override;

private:
	AABB m_box;
	std::vector<TriMeshRenderable*> m_meshes;
	Vector<MeshData> m_tessellationDatas;
	UntangleDemoView* m_pUI = nullptr;

	std::vector<float> m_meshInvMass;
};
#endif
