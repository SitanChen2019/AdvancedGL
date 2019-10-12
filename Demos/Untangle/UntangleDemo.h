#ifndef UNTANGLE_DEMO_H
#define UNTANGLE_DEMO_H

#include "idemo.h"
#include "aabb.h"

class TriMeshRenderable;

class UntangleDemo : public IDemo
{
public:
	virtual ~UntangleDemo() {};

	bool init() override;
	bool update() override;
	bool destroy() override;

private:
	AABB m_box;
	std::vector<TriMeshRenderable*> m_meshes;

};
#endif
