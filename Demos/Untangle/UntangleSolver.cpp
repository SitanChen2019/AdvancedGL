//
// Created by Administrator on 10/12/2019.
//

#include "UntangleSolver.h"
#include "GeometryMath.h"
#include "boost/container_hash/hash.hpp"
#include <iostream>

void generateMayaScriptForTriangle( Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4,Vec3 v5)
{
	printf("polyCreateFacet -ch on -tx 1 -s 1 -p %f %f %f -p %f %f %f -p %f %f %f;\n",
		v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z
	);

	printf("polyCreateFacet -ch on -tx 1 -s 1 -p %f %f %f -p %f %f %f -p %f %f %f;\n",
		v3.x, v3.y, v3.z, v4.x, v4.y, v4.z, v5.x, v5.y, v5.z
	);	
}

void generateTestCodeFOrTriangle(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4, Vec3 v5)
{
	const char* str_template = R"(bool ret = GeometryMath::isTriangleIntersect(
		Vec3(%f, %f, %f),
		Vec3(%f, %f, %f),
		Vec3(%f, %f, %f),
		Vec3(%f, %f, %f),
		Vec3(%f, %f, %f),
		Vec3(%f, %f, %f)
	);
	std::cout << ret << std::endl;)""\n";

	printf(str_template,
		v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z,
		v3.x, v3.y, v3.z, v4.x, v4.y, v4.z, v5.x, v5.y, v5.z);
}

void generateDebugCode(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4, Vec3 v5)
{
	generateMayaScriptForTriangle(v0, v1, v2, v3, v4, v5);
	generateTestCodeFOrTriangle(v0, v1, v2, v3, v4, v5);
}


UntangleSolver& UntangleSolver::singleton()
{
    static UntangleSolver s_instance;
    return s_instance;
}

void UntangleSolver::init( std::vector<Particle>&& particles, std::vector<Triangle>&& triangles )
{
    m_particles.swap( particles );
    m_triangles.swap( triangles );
	m_shareEdgeMap.clear();

	for (size_t i = 0, n = m_triangles.size(); i < n; ++i)
	{
		const Triangle& tri = m_triangles[i];

		std::array<VertexPair,3> vertexPairArray{ VertexPair{tri.p0, tri.p1}, {tri.p1, tri.p2}, {tri.p2, tri.p0} };
		std::array<int,3> localEdgeIds = { 0,1,2 };

		for ( size_t j = 0;j < vertexPairArray.size(); ++j )
		{
			const auto& vetex_pair = vertexPairArray[j];
			int localEdgeID = localEdgeIds[j];

			auto it = m_shareEdgeMap.find(vetex_pair);
			if (it == m_shareEdgeMap.end())
			{
				it = m_shareEdgeMap.insert({ vetex_pair, {} }).first;
				it->second[0] = Edge(tri.mTID, localEdgeID);
			}
			else if (it->second.at(1).isValid() == false)
			{
				it->second[1]= Edge(tri.mTID, localEdgeID);
			}
			else
			{
				//one edge can be shared at most by tow triangles
				assert(false);
			}
		}
	}
}

Edge UntangleSolver::findAdjacentTriangleEdge( const Edge& edge)
{
	Edge ret;
	auto it = m_shareEdgeMap.find({ edge.mP0, edge.mP1 });
	
	if (it == m_shareEdgeMap.end())
		return ret;

	for (auto& edgeItem : it->second)
	{
		if (edgeItem.isValid() == false)
			return ret;

		if (edgeItem.mTriangleID != edge.mTriangleID)
			return edgeItem;
	}

	return ret;
}

void UntangleSolver::preUpdate() {
    for( auto& particle: m_particles)
    {
        particle.mPrePosition =  particle.mCurPosition;
    }
}

void UntangleSolver::update()
{
    findCollisionTrianglePairs();

    calculateEdgeTriangleIntersection();

	for (auto& it : m_collisionTrianglePair)
	{
		if (it.mHitPos.size() != 2)
		{
			std::cout << "Warning Triangle Test True, Edge Test False: " << it.mTriangleID0 << " " << it.mTriangleID1 << std::endl;
		} 
	}
	groupEdgeToContour();

    calculateEdgeCorrectVector();

    correctParticles();
}

void UntangleSolver::postUpdate() {

}

void UntangleSolver::findCollisionTrianglePairs()
{
    m_collisionTrianglePair.clear();
    int triangleCount = (int)m_triangles.size();
    for( int i = 0; i < triangleCount; ++i )
    {
        for( int j = i+1; j < triangleCount ; ++j )
        {
            if( testTriangleIntersect(i,j) )
            {
                m_collisionTrianglePair.insert(TrianglePair(i,j));
            }
        }
    }
}
  
bool UntangleSolver::testTwoTriangleEdgeCollision(TrianglePair& tir_pair)
{
	bool ret = false;
	for (int i = 0; i < 3; ++i)
	{
		Edge edge(tir_pair.mTriangleID0, i);
		float hit_t = -1;
		if (testEdgeTriangleIntersect(edge, tir_pair.mTriangleID1, hit_t))
		{
			tir_pair.addHitPos(edge, hit_t);
			m_collisionEdgeTrianglePair.insert(EdgeTrianglePair(edge, tir_pair.mTriangleID1, hit_t));
			ret = true;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		Edge edge(tir_pair.mTriangleID1, i);
		float hit_t = -1;
		if (testEdgeTriangleIntersect(edge, tir_pair.mTriangleID0, hit_t))
		{
			tir_pair.addHitPos(edge, hit_t);
			m_collisionEdgeTrianglePair.insert(EdgeTrianglePair(edge, tir_pair.mTriangleID0, hit_t));
			ret = true;
		}
	}

	return ret;
}

void UntangleSolver::calculateEdgeTriangleIntersection()
{
    m_collisionEdgeTrianglePair.clear();
    for( auto it = m_collisionTrianglePair.begin(); it != m_collisionTrianglePair.end(); ++it)
    {
		TrianglePair& tir_pair = const_cast<TrianglePair&>(*it);
		testTwoTriangleEdgeCollision(tir_pair);
    }
}

void UntangleSolver::groupEdgeToContour()
{
	if (!m_isGlobalScheme)
		return;
	m_contourCorrect.clear();
	m_edgeToContour.clear();
	int contourId = -1;
	for (auto& edgeTrianglePair : m_collisionEdgeTrianglePair)
	{
		//m_edgeToContour.insert({ edgeTrianglePair, 0 });

		auto it = m_edgeToContour.find(edgeTrianglePair);
		if (it != m_edgeToContour.end())
			continue;

		contourId += 1;
		m_edgeToContour.insert({ edgeTrianglePair, contourId });
		findContour(edgeTrianglePair.mEdge.mTriangleID, edgeTrianglePair.mTriangleID, edgeTrianglePair.mEdge, contourId);

		//search in other direction
		Edge adjacentEdge = findAdjacentTriangleEdge(edgeTrianglePair.mEdge);
		EdgeTrianglePair adjacentEdgeTrianglePair = EdgeTrianglePair(adjacentEdge, edgeTrianglePair.mTriangleID, edgeTrianglePair.mHit_t);
		if (adjacentEdge.isValid() &&
			m_edgeToContour.find(adjacentEdgeTrianglePair) == m_edgeToContour.end())
		{
			m_edgeToContour.insert({ adjacentEdgeTrianglePair, contourId });
			findContour(adjacentEdge.mTriangleID, adjacentEdgeTrianglePair.mTriangleID, adjacentEdge, contourId);
		}
	}


}
void UntangleSolver::calculateEdgeCorrectVector()
{
	m_contourToVertexMap.clear();
    m_edgeCorrects.clear();
    for( const EdgeTrianglePair& edgeTri_pair : m_collisionEdgeTrianglePair )
    {
        const Edge& edge = edgeTri_pair.mEdge;

        int triangleID = edgeTri_pair.mTriangleID;
        Vec3 gvector = calculateGVector( edge,triangleID );
        
		if (m_isGlobalScheme)
		{
			int contourID = m_edgeToContour.at(edgeTri_pair);
			addContourCorrectVector(contourID, Edge(edge.mTriangleID, 0), gvector);
			addContourCorrectVector(contourID, Edge(edge.mTriangleID, 1), gvector);
			addContourCorrectVector(contourID, Edge(edge.mTriangleID, 2), gvector);

			Vec3 gvector_for_triangle = -gvector;
			addContourCorrectVector(-1 -contourID, Edge(triangleID,0), gvector_for_triangle);
			addContourCorrectVector(-1 - contourID, Edge(triangleID,1), gvector_for_triangle);
			addContourCorrectVector(-1 - contourID, Edge(triangleID,2), gvector_for_triangle);
		}
		else
		{
			addEdgeCorrectVector(Edge(edge.mTriangleID, 0), gvector);
			addEdgeCorrectVector(Edge(edge.mTriangleID, 1), gvector);
			addEdgeCorrectVector(Edge(edge.mTriangleID, 2), gvector);

			//addEdgeCorrectVector( edge , gvector );

			//Vec3 gvector_for_triangle = -gvector;
			//addEdgeCorrectVector( Edge(triangleID,0), gvector_for_triangle);
			//addEdgeCorrectVector( Edge(triangleID,1), gvector_for_triangle);
			//addEdgeCorrectVector( Edge(triangleID,2), gvector_for_triangle);
		}
    }
}

void UntangleSolver::correctParticles()
{
	if (m_isGlobalScheme)
	{
		for (const auto& contourCorrect : m_contourToVertexMap)
		{
			int contourId = contourCorrect.first;
			Vec3 direction = m_contourCorrect[contourId].getOffset();
			if (glm::length(direction) < 1e-7)
				continue;
			direction = glm::normalize(direction);

			for (int vertexId : contourCorrect.second)
			{
				m_particles[vertexId].mCurPosition += direction * m_particles[vertexId].mInvMass;
			}

		}
	}
	else
	{
		for (const auto& edgeCorrect : m_edgeCorrects)
		{
			int p0 = edgeCorrect.first.mP0;
			int p1 = edgeCorrect.first.mP1;

			Vec3 direction = edgeCorrect.second.getOffset();
			direction = glm::normalize(direction);

			m_particles[p0].mCurPosition += direction * m_particles[p0].mInvMass;
			m_particles[p1].mCurPosition += direction * m_particles[p1].mInvMass;
		}
	}
}

void UntangleSolver::addEdgeCorrectVector( const Edge& edge , const Vec3 correctVector)
{
    auto it = m_edgeCorrects.find( edge );
    if( it == m_edgeCorrects.end() )
    {
        it = m_edgeCorrects.insert( std::make_pair(edge, EdgeCorrect() )).first;
    }

    it->second.addOffset( correctVector );
}

void UntangleSolver::addContourCorrectVector(int contourId, const Edge& edge, const Vec3 correctVector)
{
	auto it1 = m_contourToVertexMap.find(contourId);
	if (it1 == m_contourToVertexMap.end())
	{
		it1 = m_contourToVertexMap.insert({ contourId, std::set<int>() }).first;
	}
	it1->second.insert(edge.mP0);
	it1->second.insert(edge.mP1);

	auto it = m_contourCorrect.find(contourId);
	if (it == m_contourCorrect.end())
	{
		it = m_contourCorrect.insert(std::make_pair(contourId, EdgeCorrect())).first;
	}
	it->second.addOffset(correctVector);
}

Edge::Edge( int triangleID , int edgeLocalID )
{
    mTriangleID = triangleID;
    mEdgeLocalID = edgeLocalID;

    const Triangle& triangle = UntangleSolver::singleton().getTriangles().at(mTriangleID);

    if( edgeLocalID == 0)
    {
        mP0 = triangle.p0;
        mP1 = triangle.p1;
    }
    else if (edgeLocalID == 1)
    {
        mP0 = triangle.p1;
        mP1 = triangle.p2;    
    }
    else if(edgeLocalID == 2)
    {
        mP0 = triangle.p2;
        mP1 = triangle.p0;
    }
    else
    {
        assert(false);
    }



}

EdgeTrianglePair::EdgeTrianglePair(const Edge& edge , int triangleID,float hit_t)
:mEdge(edge)
{
    mTriangleID = triangleID;
    mHit_t = hit_t;
}

bool UntangleSolver::testTriangleIntersect(int triangleID0, int triangleID1 )
{
    Triangle& t1 = m_triangles[triangleID0];
    Triangle& t2 = m_triangles[triangleID1];

	int shareVertex = 0;
    int sharedVertexID = -1;

    if (t1.p0 == t2.p0 || t1.p0 == t2.p1 || t1.p0 == t2.p2)
    {
        ++shareVertex;
        sharedVertexID = t1.p0;
    }
    if (t1.p1 == t2.p0 || t1.p1 == t2.p1 || t1.p1 == t2.p2)
    {
        ++shareVertex;
        sharedVertexID = t1.p1;
    }
    if (t1.p2 == t2.p0 || t1.p2 == t2.p1 || t1.p2 == t2.p2)
    {
        ++shareVertex;
        sharedVertexID = t1.p2;
    }

    if (shareVertex == 0)
    {

        bool ret1 = GeometryMath::isTriangleIntersect(
            m_particles[t1.p0].mCurPosition, m_particles[t1.p1].mCurPosition, m_particles[t1.p2].mCurPosition,
            m_particles[t2.p0].mCurPosition, m_particles[t2.p1].mCurPosition, m_particles[t2.p2].mCurPosition
        );

        //TrianglePair tri_pair(triangleID0, triangleID1);
        //bool ret2 = testTwoTriangleEdgeCollision(tri_pair);

        //if (ret1 != ret2)
        //{
        //	generateDebugCode(m_particles[t1.p0].mCurPosition, m_particles[t1.p1].mCurPosition, m_particles[t1.p2].mCurPosition,
        //		m_particles[t2.p0].mCurPosition, m_particles[t2.p1].mCurPosition, m_particles[t2.p2].mCurPosition);
        //}
        return ret1;
    }
    else if(shareVertex == 1)
    {
        int p0, p1;
        if (t2.p0 == sharedVertexID)
        {
            p0 = t2.p1;
            p1 = t2.p2;
        }
        else if (t2.p1 == sharedVertexID)
        {
            p0 = t2.p0;
            p1 = t2.p2;
        }
        else
        {
            assert(t2.p2 == sharedVertexID);
            p0 = t2.p0;
            p1 = t2.p1;
        }
        return GeometryMath::isSharedSingleVertexTriangleIntersect(
            m_particles[t1.p0].mCurPosition, m_particles[t1.p1].mCurPosition, m_particles[t1.p2].mCurPosition,
            m_particles[p0].mCurPosition, 
            m_particles[p1].mCurPosition);

    }
    else
    {
        assert(shareVertex == 2);
        return false;
    }


}

bool UntangleSolver::testEdgeTriangleIntersect(const Edge& edge, int triangleID, float& hit_t )
{
    Triangle& t1 = m_triangles[triangleID];
    return GeometryMath::edgeTriangleIntersect(
        m_particles[edge.mP0].mCurPosition, m_particles[edge.mP1].mCurPosition,
        m_particles[t1.p0].mCurPosition,m_particles[t1.p1].mCurPosition,m_particles[t1.p2].mCurPosition,
            hit_t );
}

Vec3 UntangleSolver::getTriangleNormal(int triangleID)
{
    const Triangle& t = m_triangles.at(triangleID);

    Vec3 p0 = m_particles.at(t.p0).mCurPosition;
    Vec3 p1 = m_particles.at(t.p1).mCurPosition;
    Vec3 p2 = m_particles.at(t.p2).mCurPosition;
    Vec3 normal =  glm::cross( p1 - p0, p2 - p0);
    normal = glm::normalize(normal);
    return normal;
}
Vec3 UntangleSolver::calculateGVector(const Edge& edge, int triangleID)
{
    Vec3 N = getTriangleNormal(triangleID );
    Vec3 M = getTriangleNormal(edge.mTriangleID);

    Vec3 R = glm::cross(N,M);
	Vec3 E = m_particles[edge.mP1].mCurPosition - m_particles[edge.mP0].mCurPosition;
	E = glm::normalize(E);

    Triangle B = m_triangles[ edge.mTriangleID ];
    Vec3 inner_point_in_B = 0.5f*m_particles[B.p0].mCurPosition
            + 0.25f*m_particles[B.p1].mCurPosition + 0.25f*m_particles[B.p2].mCurPosition;
    Vec3 tmp = (inner_point_in_B - m_particles[ edge.mP0].mCurPosition);
	

	//cross(tmp,E) should be equidirectional to cross(R,E)
	if (glm::dot(glm::cross(tmp, E), glm::cross(R, E)) < 0)
	{
		R = -R;
	}


	float tmp1 = glm::dot(E, R);
	float tmp2 = glm::dot(E, N);
	float tmp3 = tmp1 / tmp2;
    return R -( glm::dot(E,R)/glm::dot(E,N) ) * N;

}


void UntangleSolver::findContour(int edgeTraignleID, int triangleID, Edge currentHitedge, int contourId)
{
	auto it = m_collisionTrianglePair.find({ edgeTraignleID, triangleID });

    assert(it != m_collisionTrianglePair.end());

	assert(it->mHitPos.size() == 2);

	int currentHitIdx = 0;
	if (it->mHitPos[0].mTriangleID == currentHitedge.mTriangleID && it->mHitPos[0].mEdgeLocalID == currentHitedge.mEdgeLocalID )
	{
		currentHitIdx = 0;
	}
	else if( it->mHitPos[1].mTriangleID == currentHitedge.mTriangleID && it->mHitPos[1].mEdgeLocalID == currentHitedge.mEdgeLocalID)
	{
		currentHitIdx = 1;
	}
	else
	{
		assert(false);
	}

	int theOtherHitIdx = 1 - currentHitIdx;

 	if (it->mHitPos[theOtherHitIdx].mTriangleID == edgeTraignleID )
	{
		//two hit points on the same triangle
		Edge adjacentTriangleEdge;
		Edge hitEdge = Edge(edgeTraignleID, it->mHitPos[theOtherHitIdx].mEdgeLocalID);
		EdgeTrianglePair edgeTraignlePair = EdgeTrianglePair(hitEdge, triangleID, it->mHitPos[theOtherHitIdx].m_t);
		bool isInserted = m_edgeToContour.insert({ edgeTraignlePair, contourId }).second;
		if (!isInserted)
		{
			assert(m_edgeToContour[edgeTraignlePair] == contourId);
			return;
		}

		//replace EdgeTriangle 
		adjacentTriangleEdge = findAdjacentTriangleEdge(hitEdge);
		if (!adjacentTriangleEdge.isValid())
			return;
		edgeTraignlePair = EdgeTrianglePair(adjacentTriangleEdge, triangleID, it->mHitPos[theOtherHitIdx].m_t);
		isInserted = m_edgeToContour.insert({ edgeTraignlePair, contourId }).second;
		if (!isInserted)
		{
			//find a ring
			assert(m_edgeToContour[edgeTraignlePair] == contourId);
			return;
		}
		else
		{
			findContour(adjacentTriangleEdge.mTriangleID, triangleID, adjacentTriangleEdge, contourId);
		}
	}
	else
	{
		Edge theOtherHitEdge = Edge(it->mHitPos[theOtherHitIdx].mTriangleID, it->mHitPos[theOtherHitIdx].mEdgeLocalID);

		Edge adjacentTriangle = findAdjacentTriangleEdge(theOtherHitEdge);
		if (!adjacentTriangle.isValid())
			return;

		findContour(edgeTraignleID, adjacentTriangle.mTriangleID, adjacentTriangle, contourId);
	}
}