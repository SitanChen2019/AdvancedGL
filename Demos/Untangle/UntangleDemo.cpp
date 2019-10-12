#include "UntangleDemo.h"
#include "global.h"
#include "mesh_buffer.h"
#include "TriMeshRenderable.h"
#include "model_loader.h"

bool UntangleDemo::init()
{
	//add two triangles

    ModelLoader loader("../Demos/Untangle/res/a.obj");
    Vector<MeshData> tessellationDatas = loader.loadModel();

    for( auto& meshdata : tessellationDatas )
    {
        MeshBuffer buffers1;
        buffers1.initBufferFromMeshData(meshdata);

        TriMeshRenderable* pRenderable1 = new TriMeshRenderable(buffers1);
        Global::renderWindow().addRenderable(pRenderable1);
        m_meshes.push_back(pRenderable1);

        m_box.merge(meshdata.vertices);
    }


    std::cout << "Mesh Count: " << tessellationDatas.size() << std::endl;
    m_meshes.at(0)->setDiffuse( Vec3(0.8,0,0));
	Global::cameraControl().fitBox(m_box);


	return true;
}

bool UntangleDemo::update()
{
	return true;
}

bool UntangleDemo::destroy()
{
	for (auto pRenderable : m_meshes)
	{
		Global::renderWindow().removeRenderable(pRenderable);
		delete pRenderable;
	}

	return true;
}