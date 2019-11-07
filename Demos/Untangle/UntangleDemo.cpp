#include "UntangleDemo.h"
#include "global.h"
#include "mesh_buffer.h"
#include "TriMeshRenderable.h"
#include "model_loader.h"
#include "UntangleSolver.h"
#include <GLFW/glfw3.h>

#include "Simulator.h"
#include "Constraints.h"
#include <unordered_set>

void UntangleDemo::loadModel( std::string modelName )
{
	ModelLoader loader(modelName.c_str());
	Vector<MeshData> tessellationDatas = loader.loadModel();

	for (int i = 0; i < tessellationDatas.size(); ++i)
		optimizeMesh(tessellationDatas[i]);

    m_meshInvMass.clear();
    m_meshInvMass.resize(tessellationDatas.size(), 0.1f);
	//init solover
    initSimulationSolver(tessellationDatas);
    initUntangleSolver(tessellationDatas);
    

	m_box = AABB();
	for (auto& meshdata : tessellationDatas)
	{
		MeshBuffer buffers1;
		buffers1.initBufferFromMeshData(meshdata);

		TriMeshRenderable* pRenderable1 = new TriMeshRenderable(buffers1);
		Global::renderWindow().addRenderable(pRenderable1);
		m_meshes.push_back(pRenderable1);

		m_box.merge(meshdata.vertices);
	}

	std::cout << "Mesh Count: " << tessellationDatas.size() << std::endl;
	m_meshes.at(0)->setDiffuse(Vec3(0.8, 0, 0));
    
    if( m_meshes.size() > 1)
        m_meshes.at(1)->setDiffuse(Vec3(0.8, 0.8, 0));
	Global::cameraControl().fitBox(m_box);

	m_tessellationDatas.swap(tessellationDatas);

    m_autoRun = false;
}


void UntangleDemo::initSimulationSolver(const Vector<MeshData>& tessellationDatas)
{
    Vector<PBD::Particle> particles;
    Vector<PBD::Triangle> triangles;
    int vertex_offset = 0;
    int pid = 0;
    int tid = 0;
    int meshID = 0;

    for (auto& meshdata : tessellationDatas)
    {
        for (auto& pos : meshdata.vertices)
        {
            PBD::Particle p;
            p.mCurPos = pos;
            p.mPrePos = pos;
            p.mInvMass = m_meshInvMass[meshID];
            p.mVelocity = Vec3(0);
            particles.push_back(p);
        }

        for (size_t i = 0; i < meshdata.indices.size(); i += 3)
        {
            PBD::Triangle t;
            t.p0 = meshdata.indices[i] + vertex_offset;
            t.p1 = meshdata.indices[i + 1] + vertex_offset;
            t.p2 = meshdata.indices[i + 2] + vertex_offset;
            triangles.push_back(t);
        }

        vertex_offset += (int)meshdata.vertices.size();
        ++meshID;
    }

    //init constraint
    std::unordered_set<size_t> processedEdges;
    std::vector<PBD::Constraint> vecConstrains;
    vecConstrains.reserve(triangles.size());

    for (const auto triangle : triangles)
    {
        auto getEdgeUniqueId = [](int vid0, int vid1) -> size_t
        {
            size_t seed = vid0 < vid1 ? vid0 : vid1;
            boost::hash_combine(seed, vid0 < vid1 ? vid1 : vid0);
            return seed;
        };

        for (auto vertexPair :
            std::initializer_list<std::pair<int, int>>{ {triangle.p0,triangle.p1},{triangle.p0,triangle.p2},{triangle.p1,triangle.p2} })
        {
            int vid0 = vertexPair.first;
            int vid1 = vertexPair.second;
            size_t edgeID = getEdgeUniqueId(vid0,vid1);
            if (processedEdges.find(edgeID) == processedEdges.end())
            {
                processedEdges.insert(edgeID);

                float distance = glm::distance( particles.at(vid0).mCurPos, 
                    particles.at(vid1).mCurPos );
                PBD::DistanceConstraint dc(vid0, vid1, distance);
                vecConstrains.push_back(PBD::Constraint(dc));
            }
        }
    }

    PBD::Simualtor::singleton().init(std::move(particles), std::move(triangles), 
        std::move(vecConstrains));
}

void UntangleDemo::initUntangleSolver(const Vector<MeshData>& tessellationDatas)
{
    Vector<Particle> particles;
    Vector<Triangle> triangles;
    int vertex_offset = 0;
    int pid = 0;
    int tid = 0;
    int meshID = 0;

    for (auto& meshdata : tessellationDatas)
    {
        for (auto& pos : meshdata.vertices)
        {
            Particle p;
            p.mCurPosition = pos;
            p.mPrePosition = pos;
            p.mPID = pid++;
            p.mInvMass = m_meshInvMass[meshID];
            particles.push_back(p);
        }

        for (size_t i = 0; i < meshdata.indices.size(); i += 3)
        {
            Triangle t;
            t.p0 = meshdata.indices[i] + vertex_offset;
            t.p1 = meshdata.indices[i + 1] + vertex_offset;
            t.p2 = meshdata.indices[i + 2] + vertex_offset;
            t.mTID = tid++;

            triangles.push_back(t);
        }

        vertex_offset += (int)meshdata.vertices.size();
        ++meshID;
    }

    UntangleSolver::singleton().init(std::move(particles), std::move(triangles));
}

bool UntangleDemo::init()
{
	//add two triangles

	m_pUI = new UntangleDemoView(this);
	Global::uiManager().addUIView(m_pUI);

	loadModel(Global::DemoPath("/Untangle/res/a.obj"));
	return true;
}


int UntangleDemo::getMeshCount()
{
	return (int)m_tessellationDatas.size();
}

void UntangleDemo::setMeshInvMass(int meshID, float value)
{
	if (meshID < m_meshInvMass.size())
	{
		m_meshInvMass[meshID] = value;

		size_t vertexOffset = 0;
		for (int i = 0; i < meshID; ++i)
		{
			vertexOffset += m_tessellationDatas[i].vertices.size();
		}
		UntangleSolver::singleton().updateParticleInvMass(vertexOffset, vertexOffset + m_tessellationDatas[meshID].vertices.size(), value);
	}
}

float UntangleDemo::getMeshInvMass(int meshID)
{
	if (meshID < m_meshInvMass.size())
	{
		return m_meshInvMass[meshID];
	}
	return 0;
}



void UntangleDemo::reloadModel(std::string modelName)
{
	for (auto pRenderable : m_meshes)
	{
		Global::renderWindow().removeRenderable(pRenderable);
		delete pRenderable;
	}
	m_meshes.clear();

	std::string fullName = std::string("/Untangle/res/") + modelName + std::string(".obj");
	loadModel(Global::DemoPath(fullName));
}

bool UntangleDemo::isGlobalScheme()
{
	return  UntangleSolver::singleton().isGlobalScheme();
}

void UntangleDemo::setGlobalScheme(bool bValue)
{
	UntangleSolver::singleton().setGlobalScheme(bValue);
}

void UntangleDemo::optimizeMesh(MeshData& meshData)
{
	auto checkTwoVertexOverlap = [](const Vec3& p0, const Vec3& p1)
	{
		const float E = 1e-7f;
		if (fabs(p0.x - p1.x) < E
			&& fabs(p0.y - p1.y) < E
			&& fabs(p0.z - p1.z) < E
			)
			return true;
		return false;
	};

	Vec3Sequence  newVertexList;
	newVertexList.reserve(meshData.vertices.size());
	std::map<int, int> indexConvertMap;
	for (int i = 0, n = (int)meshData.vertices.size(); i < n; ++i)
	{
		bool bOverlap = false;
		for (int j = 0, m = (int)newVertexList.size(); j < m; ++j)
		{
			if (checkTwoVertexOverlap(meshData.vertices[i], newVertexList[j]))
			{
				bOverlap = true;
				indexConvertMap.insert({ i,j });
				break;
			}
		}

		if (!bOverlap)
		{
			indexConvertMap.insert({ i,(int)newVertexList.size() });
			newVertexList.push_back(meshData.vertices[i]);
		}
	};

	if (newVertexList.size() == meshData.vertices.size())
		//no overlap
		return;

	Int32Sequence  newIndexSeq;
	newIndexSeq.reserve(meshData.indices.size());
	for (auto oldIndex : meshData.indices)
	{
		newIndexSeq.push_back(indexConvertMap.at(oldIndex));
	}

	meshData.vertices.swap(newVertexList);
	meshData.indices.swap(newIndexSeq);
	meshData.normals.clear();
	meshData.textCoors.clear();

}

void UntangleDemo::switchRun()
{
    m_autoRun = !m_autoRun;
}

void UntangleDemo::exeOneStep()
{
    std::vector<PBD::Particle>&  vecSimParticle = PBD::Simualtor::singleton().getParticles();
    std::vector<Particle>& vecUntangleParticle = UntangleSolver::singleton().getParticles();
    for (size_t i = 0, n = vecSimParticle.size(); i < n; ++i)
    {
        vecSimParticle[i].mCurPos = vecUntangleParticle[i].mCurPosition;
    }
    PBD::Simualtor::singleton().preUpdate();
    //PBD::Simualtor::singleton().update();
    PBD::Simualtor::singleton().postUpdate();


    for (size_t i = 0, n = vecSimParticle.size(); i < n; ++i)
    {
        vecUntangleParticle[i].mCurPosition = vecSimParticle[i].mCurPos;
    }
    UntangleSolver::singleton().preUpdate();
    UntangleSolver::singleton().update();
    UntangleSolver::singleton().postUpdate();
}

bool UntangleDemo::update()
{
    bool bSpacePressed = false;
    
    if (!m_autoRun)
    {
        bSpacePressed = glfwGetKey(Global::renderWindow().getGLFWWindow(), GLFW_KEY_SPACE);
    }
    else
    {
        exeOneStep();
    }

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
	m_meshes.clear();

	Global::uiManager().removeUIView(m_pUI);
	delete m_pUI;
	m_pUI = nullptr;

	return true;
}

EventHandleStatus  UntangleDemo::handleMouseInput(int button, int action, int mods)
{
   
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {

            GLFWwindow* window = Global::renderWindow().getGLFWWindow();
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            {
                if (pickVertex())
                    return EventHandled;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                if (pickTriangle())
                    return EventHandled;
            }
        }
    }
    return EventUnHandled;
}


bool UntangleDemo::pickVertex()
{
    GLFWwindow* window = Global::renderWindow().getGLFWWindow();
    const RenderWindow& renderWindow = Global::renderWindow();

    double dX, dY;
    glfwGetCursorPos(window, &dX, &dY);
    float x = (float)dX;
    float y = (float)dY;
    const float radius = 5; //px
    Vector<unsigned> pickVertex = Global::mousePicking().pickVertex<Particle>(x, y, radius,
        (float)renderWindow.getWinWidth(), (float)renderWindow.getWinHeight(),
        renderWindow.getViewMatrix(), renderWindow.getProjMatrix(),
        UntangleSolver::singleton().getParticles(), [](const Particle& p) {return p.mCurPosition; }
    );

    if (pickVertex.empty() == false)
    {
        std::cout << "Pick particle " << pickVertex.front() << std::endl;
        return true;
    }
    else
        return false;
 
}

bool UntangleDemo::pickTriangle()
{
    GLFWwindow* window = Global::renderWindow().getGLFWWindow();
    const RenderWindow& renderWindow = Global::renderWindow();

    double dX, dY;
    glfwGetCursorPos(window, &dX, &dY);
    float x = (float)dX;
    float y = (float)dY;
    const float radius = 5; //px
    Vector<unsigned> pickedTriangles = Global::mousePicking().pickTriangle<Particle, Triangle>(x, y, radius,
        (float)renderWindow.getWinWidth(), (float)renderWindow.getWinHeight(),
        renderWindow.getViewMatrix(), renderWindow.getProjMatrix(),
        UntangleSolver::singleton().getParticles(), [](const Vector<Particle>& particles, size_t i) {return particles.at(i).mCurPosition; },
        UntangleSolver::singleton().getTriangles(), UntangleSolver::singleton().getTriangles().size(),
        [](const std::vector<Triangle>& triIndices, size_t i) { return std::tuple<int, int, int>(triIndices[i].p0, triIndices[i].p1, triIndices[i].p2); }
    );

    if (pickedTriangles.empty() == false)
    {
        unsigned pickTriangleId = pickedTriangles.front();
        const Triangle& tri = UntangleSolver::singleton().getTriangles().at(pickTriangleId);
        std::cout << "Pick particle ID " << pickTriangleId << "(" << tri.p0 << " "<<  tri.p1 << " " << tri.p2 << ")" << std::endl;
        return true;
    }
    else
        return false;

}