#include "UntangleDemo.h"
#include "global.h"
#include "mesh_buffer.h"
#include "TriMeshRenderable.h"
#include "model_loader.h"
#include "UntangleSolver.h"
#include <GLFW/glfw3.h>


bool UntangleDemo::init()
{
	//add two triangles

	m_pUI = new UntangleDemoView(this);
	Global::uiManager().addUIView(m_pUI);

    ModelLoader loader("../Demos/Untangle/res/e.obj");
    Vector<MeshData> tessellationDatas = loader.loadModel();

    //init solover
    Vector<Particle> particles;
    Vector<Triangle> triangles;
    int vertex_offset = 0;
    int pid = 0;
    int tid = 0;
	int meshID = 0;
    for(auto& meshdata : tessellationDatas )
    {
        for( auto& pos : meshdata.vertices )
        {
            Particle p;
            p.mCurPosition = pos;
            p.mPrePosition = pos;
            p.mPID = pid++;
            p.mInvMass = meshID == 1 ? 0 :  0.1f;
            particles.push_back( p );
        }

        for( size_t i = 0 ; i < meshdata.indices.size(); i+=3 )
        {
            Triangle t;
            t.p0 = meshdata.indices[i] + vertex_offset;
            t.p1 = meshdata.indices[i+1] + vertex_offset;
            t.p2 = meshdata.indices[i+2] + vertex_offset;
            t.mTID = tid++;

            triangles.push_back(t);
        }

		vertex_offset += (int)meshdata.vertices.size();
		++meshID;
    }
    UntangleSolver::singleton().init( std::move( particles), std::move(triangles) );

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
	m_meshes.at(1)->setDiffuse(Vec3(0.8, 0.8, 0));
	Global::cameraControl().fitBox(m_box);

	m_tessellationDatas.swap(tessellationDatas);

	return true;
}
void UntangleDemo::exeOneStep()
{
    UntangleSolver::singleton().preUpdate();
    UntangleSolver::singleton().update();
    UntangleSolver::singleton().postUpdate();
}

bool UntangleDemo::update()
{
	bool bSpacePressed = glfwGetKey(Global::renderWindow().getGLFWWindow(), GLFW_KEY_SPACE);

	//copy the vertex to render
	Particle* pParticle = UntangleSolver::singleton().getParticles().data();
	for (size_t j = 0; j < m_tessellationDatas.size(); ++j)
	{
		const auto& tesseraData = m_tessellationDatas.at(j);
		Vec3Sequence newVertexSeq;
		newVertexSeq.reserve(tesseraData.vertices.size());
		for (size_t i = 0; i < tesseraData.vertices.size(); ++i, ++pParticle)
		{
			newVertexSeq.push_back(bSpacePressed ? pParticle->mPrePosition : pParticle->mCurPosition);
		}
		m_meshes[j]->updateVertices(newVertexSeq);
	}

	return true;
}

bool UntangleDemo::destroy()
{
	for (auto pRenderable : m_meshes)
	{
		Global::renderWindow().removeRenderable(pRenderable);
		delete pRenderable;
	}

	Global::uiManager().removeUIView(m_pUI);
	delete m_pUI;
	m_pUI = nullptr;

	return true;
}