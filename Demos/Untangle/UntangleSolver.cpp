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

std::list<Edge> UntangleSolver::findAdjacentTriangleEdges( const Edge& edge)
{
	std::list<Edge> ret;
	auto it = m_shareEdgeMap.find({ edge.mP0, edge.mP1 });
	
	if (it == m_shareEdgeMap.end())
		return ret;

	for (auto& edgeItem : it->second)
	{
		if (edgeItem.isValid() == false)
			return ret;

		if (edgeItem.mTriangleID != edge.mTriangleID)
            ret.push_back(edgeItem);
	}

	return ret;
}

std::vector<int> UntangleSolver::findVertexAdjacentTriangle(int vertexID)
{
    std::vector<int> tris;
    tris.reserve(8);


    for (int i = 0, n = (int)m_triangles.size(); i <n; ++i)
    {
        const auto& tri = m_triangles.at(i);
        if (tri.p0 == vertexID || tri.p1 == vertexID || tri.p2 == vertexID)
            tris.push_back(i);
    }

    return tris;
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
	groupHitPositionToContour();

    //calculateEdgeCorrectVector();

    //correctParticles();
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
            auto intersect_type = testTriangleIntersect(i, j);
            if(intersect_type != NO_INTERSECTION )
            {
                m_collisionTrianglePair.insert(TrianglePair(i,j));
            }
        }
    }
}
  
int UntangleSolver::getShareVertexCount(int triangleID0, int triangleID1) const
{
    assert(triangleID0 != triangleID1);
    const Triangle& t1 = m_triangles[triangleID0];
    const Triangle& t2 = m_triangles[triangleID1];

    int shareVertex = 0;
    if (t1.p0 == t2.p0 || t1.p0 == t2.p1 || t1.p0 == t2.p2)
    {
        ++shareVertex;
    }
    if (t1.p1 == t2.p0 || t1.p1 == t2.p1 || t1.p1 == t2.p2)
    {
        ++shareVertex;
    }
    if (t1.p2 == t2.p0 || t1.p2 == t2.p1 || t1.p2 == t2.p2)
    {
        ++shareVertex;
    }

    return shareVertex;
}


std::array<int, 3> UntangleSolver::rerangeTriangleVertexListByShare(int triangleID0, int triangleID1) const
{
    assert(triangleID0 != triangleID1);
    const Triangle& t1 = m_triangles[triangleID0];
    const Triangle& t2 = m_triangles[triangleID1];

    bool p0Shared = false;
    bool p1Shared = false;
    bool p2Shared = false;

    std::array<int, 3> ret;
    int currentIdx = 0;

    if (t1.p0 == t2.p0 || t1.p0 == t2.p1 || t1.p0 == t2.p2)
    {
        ret[currentIdx++] = t1.p0;
        p0Shared = true;
    }
    if (t1.p1 == t2.p0 || t1.p1 == t2.p1 || t1.p1 == t2.p2)
    {
        ret[currentIdx++] = t1.p1;
        p1Shared = true;
    }
    if (t1.p2 == t2.p0 || t1.p2 == t2.p1 || t1.p2 == t2.p2)
    {
        ret[currentIdx++] = t1.p2;
        p2Shared = true;
    }

    if (p0Shared == false)
    {
        ret[currentIdx++] = t1.p0;
    }
    if (p1Shared == false)
    {
        ret[currentIdx++] = t1.p1;
    }
    if (p2Shared == false)
    {
        ret[currentIdx++] = t1.p2;
    }

    assert(currentIdx == 3);
    return ret;
}

bool UntangleSolver::testTwoTriangleEdgeCollision(TrianglePair& tri_pair)
{
    int sharedVertexCount = getShareVertexCount(tri_pair.mTriangleID0, tri_pair.mTriangleID1);
    bool ret = false;

    if (sharedVertexCount == 0)
    {
        for (int i = 0; i < 3; ++i)
        {
            Edge edge(tri_pair.mTriangleID0, i);
            float hit_t = -1;
            if (testEdgeTriangleIntersect(edge, tri_pair.mTriangleID1, hit_t))
            {
                TriangleHitPoint hitPos(edge, hit_t, tri_pair.mTriangleID1);
                m_TriangleHitPositions.emplace(hitPos.hash(), hitPos);
                tri_pair.addHitPos(hitPos);
                ret = true;
            }
        }

        for (int i = 0; i < 3; ++i)
        {
            Edge edge(tri_pair.mTriangleID1, i);
            float hit_t = -1;
            if (testEdgeTriangleIntersect(edge, tri_pair.mTriangleID0, hit_t))
            {
                TriangleHitPoint hitPos(edge, hit_t, tri_pair.mTriangleID0);
                m_TriangleHitPositions.emplace(hitPos.hash(), hitPos);
                tri_pair.addHitPos(hitPos);
                ret = true;
            }
        }
    }
    else if (sharedVertexCount == 1)
    {
        std::array<int, 3> rerangeVertexList = rerangeTriangleVertexListByShare(tri_pair.mTriangleID0, tri_pair.mTriangleID1);
        Edge edge(tri_pair.mTriangleID0, rerangeVertexList[1], rerangeVertexList[2]);
        float hit_t = -1;
        if (testEdgeTriangleIntersect(edge, tri_pair.mTriangleID1, hit_t))
        {
            TriangleHitPoint hitPos(edge, hit_t, tri_pair.mTriangleID1);
            m_TriangleHitPositions.emplace(hitPos.hash(), hitPos);
            tri_pair.addHitPos(hitPos);

            ret = true;
        }

        if (ret == false)
        {
            rerangeVertexList = rerangeTriangleVertexListByShare(tri_pair.mTriangleID1, tri_pair.mTriangleID0);
            edge = Edge(tri_pair.mTriangleID1, rerangeVertexList[1], rerangeVertexList[2]);
            hit_t = -1;
            if (testEdgeTriangleIntersect(edge, tri_pair.mTriangleID0, hit_t))
            {
                TriangleHitPoint hitPos(edge, hit_t, tri_pair.mTriangleID0);
                m_TriangleHitPositions.emplace(hitPos.hash(), hitPos);
                tri_pair.addHitPos(hitPos);
                ret = true;
            }
        }

        if (ret)
        {
            TriangleHitPoint hitPos(rerangeVertexList[0], tri_pair.mTriangleID0, tri_pair.mTriangleID1);
            m_TriangleHitPositions.emplace(hitPos.hash(), hitPos);
            tri_pair.addHitPos(hitPos);
        }
    }
    else
    {
        assert(false);
    }

    return ret;
}

void UntangleSolver::calculateEdgeTriangleIntersection()
{
    m_TriangleHitPositions.clear();
    for( auto it = m_collisionTrianglePair.begin(); it != m_collisionTrianglePair.end(); ++it)
    {
		TrianglePair& tri_pair = const_cast<TrianglePair&>(*it);
		testTwoTriangleEdgeCollision(tri_pair);
    }
}

void UntangleSolver::groupHitPositionToContour()
{
	if (!m_isGlobalScheme)
		return;
	m_contourCorrect.clear();
	m_hitPosToGraph.clear();

    int graphID = -1;
    for (auto& hitPosIter : m_TriangleHitPositions)
    {
        auto& hitPos = hitPosIter.second;
        auto graphIt = m_hitPosToGraph.find(hitPos.hash());

        if (hitPos.mType == SHARED_VERTEX)
           continue;

        TriangleHitPoint curEdgeHitPoint = hitPos;

        if (graphIt != m_hitPosToGraph.end())
            continue;

        TriangleHitPoint pairHitPos = findPairHitPos(hitPos);

        //a new graph
        graphID += 1;
        m_hitPosToGraph.insert({ hitPos.hash(), graphID });
        m_hitPosToGraph.insert({ pairHitPos.hash(), graphID });

        for (const auto& curhitPos : std::initializer_list<TriangleHitPoint>{ hitPos, pairHitPos })
        {
            if (curhitPos.mType == EDGE_POINT)
            {
                Edge currentEdge(curhitPos.mTriangleID, curhitPos.mEdgeLocalID);
                std::list<Edge> adjacentEdges = findAdjacentTriangleEdges(currentEdge);
                for (const auto& adj_edge : adjacentEdges)
                {
                    findContour2(curhitPos.mTriangleID, curhitPos.mHitTriangleID, adj_edge.mTriangleID, curhitPos, graphID);
                }
            }
            else
            {
                int cur_src_triangleID = curhitPos.mTriangleID0;
                int cur_hit_triangleID = curhitPos.mTriangleID1;
                if (cur_hit_triangleID != curEdgeHitPoint.mHitTriangleID)
                    std::swap(cur_src_triangleID, cur_hit_triangleID);

                std::vector<int> adjacnetTriangles = findVertexAdjacentTriangle(curhitPos.mVertexID);
                for (int triID : adjacnetTriangles)
                {
                    if (triID == cur_src_triangleID || triID == cur_hit_triangleID)
                        continue;
                    TrianglePair tri_pair = TrianglePair(triID, cur_hit_triangleID);
                    if (m_collisionTrianglePair.find(tri_pair) == m_collisionTrianglePair.end())
                        continue;

                    findContour2(cur_src_triangleID, cur_hit_triangleID, triID, curhitPos, graphID);
                }
            }

        }
    }

	//int contourId = -1;
	//for (auto& hitPosIter : m_TriangleHitPositions)
	//{
	//	//m_edgeToContour.insert({ edgeTrianglePair, 0 });


	//	if (it != m_hitPosToContour.end())
	//		continue;

 //       if (hitPos.mType == SHARED_VERTEX)
 //           continue;

	//	contourId += 1;
	//	m_hitPosToContour.insert({ hitPos.hash(), contourId });
	//	findContour(edgeTrianglePair.mEdge.mTriangleID, edgeTrianglePair.mTriangleID, edgeTrianglePair.mEdge, contourId);

	//	//search in other direction
	//	Edge adjacentEdge = findAdjacentTriangleEdge(edgeTrianglePair.mEdge);
	//	EdgeTrianglePair adjacentEdgeTrianglePair = EdgeTrianglePair(adjacentEdge, edgeTrianglePair.mTriangleID, edgeTrianglePair.mHit_t);
	//	if (adjacentEdge.isValid() &&
	//		m_hitPosToContour.find(adjacentEdgeTrianglePair) == m_hitPosToContour.end())
	//	{
	//		m_hitPosToContour.insert({ adjacentEdgeTrianglePair, contourId });
	//		findContour(adjacentEdge.mTriangleID, adjacentEdgeTrianglePair.mTriangleID, adjacentEdge, contourId);
	//	}
	//}

    std::cout << "Debug: find contour graph  " << graphID+1 << std::endl;
}
void UntangleSolver::calculateEdgeCorrectVector()
{
	m_contourToVertexMap.clear();
    m_edgeCorrects.clear();
    for( const auto& hitPosIter : m_TriangleHitPositions )
    {
        const auto& hitPos = hitPosIter.second;
        if (hitPos.mType == SHARED_VERTEX)
            continue;

        Edge edge(hitPos.mTriangleID, hitPos.mEdgeLocalID);
        int triangleID = hitPos.mHitTriangleID;
        Vec3 gvector = calculateGVector( edge,triangleID );
        
		if (m_isGlobalScheme)
		{
			int contourID = m_hitPosToGraph.at(hitPosIter.first);
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


			Vec3 gvector_for_triangle = -gvector;
			addEdgeCorrectVector( Edge(triangleID,0), gvector_for_triangle);
			addEdgeCorrectVector( Edge(triangleID,1), gvector_for_triangle);
			addEdgeCorrectVector( Edge(triangleID,2), gvector_for_triangle);
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

Edge::Edge(int triangleID, int p0, int p1)
{
    mTriangleID = triangleID;
    const Triangle& triangle = UntangleSolver::singleton().getTriangles().at(mTriangleID);

    if( (p0 == triangle.p0 && p1== triangle.p1) || (p0 == triangle.p1 && p1 == triangle.p0) )
    {
        mEdgeLocalID = 0;
        mP0 = triangle.p0;
        mP1 = triangle.p1;
    }
    else if ((p0 == triangle.p1 && p1 == triangle.p2) || (p0 == triangle.p2 && p1 == triangle.p1))
    {
        mP0 = triangle.p1;
        mP1 = triangle.p2;
        mEdgeLocalID = 1;
    }
    else if ((p0 == triangle.p0 && p1 == triangle.p2) || (p0 == triangle.p2 && p1 == triangle.p0))
    {
        mP0 = triangle.p2;
        mP1 = triangle.p0;
        mEdgeLocalID = 2;
    }
    else
    {
        assert(false);
    }

}




TriangleIntersectType UntangleSolver::testTriangleIntersect(int triangleID0, int triangleID1 )
{
    Triangle& t1 = m_triangles[triangleID0];
    Triangle& t2 = m_triangles[triangleID1];

    if (triangleID0 == 853 && triangleID1 == 1636)
    {
        //generateDebugCode(m_particles[t1.p0].mCurPosition, 
        //                    m_particles[t1.p1].mCurPosition, 
        //                    m_particles[t1.p2].mCurPosition,
        //                    m_particles[t2.p0].mCurPosition, 
        //                    m_particles[t2.p1].mCurPosition, 
        //                    m_particles[t2.p2].mCurPosition);

        bool ret = GeometryMath::isTriangleIntersect(
            Vec3(0.028800, -0.430300, 0.122500),
            Vec3(0.006091, -0.434383, 0.103956),
            Vec3(0.023555, -0.479317, 0.114574),
            Vec3(0.012833, -0.474400, 0.106400),
            Vec3(0.022169, -0.416075, 0.105390),
            Vec3(0.006433, -0.457500, 0.122400)
        );
        std::cout << ret << std::endl;

    }
	int shareVertex = getShareVertexCount(triangleID0, triangleID1);
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
        return ret1 ? NO_SHARE_INTERSECTION : NO_INTERSECTION;
    }
    else if(shareVertex == 1)
    {

        auto rerangeIndices0 = rerangeTriangleVertexListByShare(triangleID0, triangleID1);
        auto rerangeIndices1 = rerangeTriangleVertexListByShare(triangleID1, triangleID0);
        size_t p0 = (size_t)rerangeIndices1[1];
        size_t p1 = (size_t)rerangeIndices1[2];

        bool ret = GeometryMath::isSharedSingleVertexTriangleIntersect(
            m_particles[rerangeIndices0[0]].mCurPosition, 
            m_particles[rerangeIndices0[1]].mCurPosition, 
            m_particles[rerangeIndices0[2]].mCurPosition,
            m_particles[p0].mCurPosition, 
            m_particles[p1].mCurPosition);

        return ret ? SINGLE_SHARE_INTERSECTION : NO_INTERSECTION;

    }
    else
    {
        assert(shareVertex == 2);
        return NO_INTERSECTION;
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

TriangleHitPoint UntangleSolver::findPairHitPos(const TriangleHitPoint& hitPos)
{
    auto tri_pair_ids = hitPos.getTwoTriangles();
    TrianglePair tri_pir(tri_pair_ids.first, tri_pair_ids.second);
    auto itor = m_collisionTrianglePair.find(tri_pir);
    assert(itor != m_collisionTrianglePair.end());
    assert(itor->mHitPos.size() == 2);
    for (const auto& curHitPos : itor->mHitPos)
    {
        if (curHitPos.hash() == hitPos.hash() )
            continue;
        return curHitPos;
    }
    //should not arrive here
    assert(false);
    return hitPos;
}

void UntangleSolver::findContour2(int src_triangleID, int hit_triangleID, int target_triangleID, const TriangleHitPoint& src_hitPos, int graphID)
{
    TrianglePair tri_pair(target_triangleID, hit_triangleID);
    auto itor = m_collisionTrianglePair.find(tri_pair);
    assert(itor != m_collisionTrianglePair.end());
   
    //match hitPos
    const TrianglePair& collisionTrianglePair = *itor;
    int matchIndex = -1;
    for (int i = 0, n = (int) collisionTrianglePair.mHitPos.size(); i <n; ++i)
    {
        const auto& hitPos = collisionTrianglePair.mHitPos.at(i);
        if (isHitPosMatch(src_hitPos,  hitPos))
        {
            matchIndex = i;
        }
    }

    assert(matchIndex == 0 || matchIndex == 1);
    TriangleHitPoint curHitPos = collisionTrianglePair.mHitPos[matchIndex];
    TriangleHitPoint curPairHitPos = collisionTrianglePair.mHitPos[2-1-matchIndex];

    if (m_hitPosToGraph.find(curHitPos.hash()) != m_hitPosToGraph.end())
        return;

    assert(m_hitPosToGraph.find(curPairHitPos.hash()) == m_hitPosToGraph.end());
    m_hitPosToGraph.emplace(curHitPos.hash(), graphID);
    m_hitPosToGraph.emplace(curPairHitPos.hash(), graphID);

    if (curPairHitPos.mType == EDGE_POINT)
    {
        Edge currentEdge(curPairHitPos.mTriangleID, curPairHitPos.mEdgeLocalID);
        std::list<Edge> adjacentEdges = findAdjacentTriangleEdges(currentEdge);
        for (const auto& adj_edge : adjacentEdges)
        {
            findContour2( curPairHitPos.mTriangleID, curPairHitPos.mHitTriangleID, adj_edge.mTriangleID, curPairHitPos, graphID );
        }
    }
    else
    {
        int cur_src_triangleID = curPairHitPos.mTriangleID0;
        int cur_hit_triangleID = curPairHitPos.mTriangleID1;
        if (cur_hit_triangleID != hit_triangleID)
            std::swap(cur_src_triangleID, cur_hit_triangleID);

        std::vector<int> adjacnetTriangles = findVertexAdjacentTriangle(curPairHitPos.mVertexID);
        for (int triID : adjacnetTriangles)
        {
            if ( triID == cur_src_triangleID || triID == cur_hit_triangleID)
                continue;
            tri_pair = TrianglePair(triID, hit_triangleID);
            if (m_collisionTrianglePair.find(tri_pair) == m_collisionTrianglePair.end())
                continue;

            findContour2(cur_src_triangleID, cur_hit_triangleID, triID, curPairHitPos, graphID);
        }
    }

}

bool UntangleSolver::isHitPosMatch(const TriangleHitPoint& hitPos1, const TriangleHitPoint& hitPos2)
{
    if (hitPos1.mType != hitPos2.mType)
        return false;

    if (hitPos1.mType == EDGE_POINT)
    {
        Edge edge1(hitPos1.mTriangleID, hitPos1.mEdgeLocalID);
        Edge edge2(hitPos2.mTriangleID, hitPos2.mEdgeLocalID);
        if (edge1.mP0 == edge2.mP0 && edge1.mP1 == edge2.mP1)
            return true;
        if (edge1.mP0 == edge2.mP1 && edge1.mP1 == edge2.mP0)
            return true;
        return false;
    }
    else
    {
        return hitPos1.mVertexID == hitPos2.mVertexID;
    }
}


void UntangleSolver::findContour(int edgeTraignleID, int triangleID, Edge currentHitedge, int contourId)
{/*
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
		bool isInserted = m_hitPosToContour.insert({ edgeTraignlePair, contourId }).second;
		if (!isInserted)
		{
			assert(m_hitPosToContour[edgeTraignlePair] == contourId);
			return;
		}

		//replace EdgeTriangle 
		adjacentTriangleEdge = findAdjacentTriangleEdge(hitEdge);
		if (!adjacentTriangleEdge.isValid())
			return;
		edgeTraignlePair = EdgeTrianglePair(adjacentTriangleEdge, triangleID, it->mHitPos[theOtherHitIdx].m_t);
		isInserted = m_hitPosToContour.insert({ edgeTraignlePair, contourId }).second;
		if (!isInserted)
		{
			//find a ring
			assert(m_hitPosToContour[edgeTraignlePair] == contourId);
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
    */
}