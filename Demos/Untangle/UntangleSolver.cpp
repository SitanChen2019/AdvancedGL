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

void UntangleSolver::init( std::vector<Particle>&& particles,
                           std::vector<Triangle>&& triangles)
{

    bool ret = GeometryMath::isTriangleIntersect(
        particles[0].mCurPosition,
        particles[1].mCurPosition,
        particles[2].mCurPosition,
        particles[3].mCurPosition,
        particles[4].mCurPosition,
        particles[5].mCurPosition
    );
    std::cout << ret << std::endl;

    bool ret2 = REF::isTriangleIntersect(
        particles[0].mCurPosition,
        particles[1].mCurPosition,
        particles[2].mCurPosition,
        particles[3].mCurPosition,
        particles[4].mCurPosition,
        particles[5].mCurPosition,
            2, 1, 0,
            7458, 7459, 7460
        );

    GeometryMath::detectIntersection3(
        particles[0].mCurPosition,
        particles[1].mCurPosition,
        particles[2].mCurPosition,
        particles[3].mCurPosition,
        particles[4].mCurPosition,
        particles[5].mCurPosition
        );

    m_particles.swap( particles );
    m_triangles.swap( triangles );
	m_shareEdgeMap.clear();
    m_vertexUpdateFlags.resize(particles.size());
    m_vertexAccumOffset.resize(m_particles.size(), Vec3(0));

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

void UntangleSolver::clearTmpData()
{
    m_collisionTrianglePair.clear();
    m_TriangleHitPositions.clear();
    m_GrpahToHitPosMap.clear();
    m_ContourToHitPosMap.clear();
    m_familtyHpMaps.clear();
    m_vertexAccumOffset.assign(m_particles.size(), Vec3(0));
}

void UntangleSolver::update()
{
    if (m_isGlobalScheme)
        update_global();
    else
        update_local();
}

void UntangleSolver::update_local()
{
    findCollisionTrianglePairs();
    calculateEdgeTriangleIntersection();

    minimizeLocalContour();

    //apply offset
    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        Vec3 offset = m_vertexAccumOffset[i];
        m_particles[i].mCurPosition += offset;
    }

    clearTmpData();
}

void UntangleSolver::update_global()
{
    

    findCollisionTrianglePairs();
    calculateEdgeTriangleIntersection();

	for (auto& it : m_collisionTrianglePair)
	{
		if (it.mHitPos.size() != 2)
		{
			std::cout << "Warning Triangle Test True, Edge Test False: " << it.mTriangleID0 << " " << it.mTriangleID1 << std::endl;

            //debugTriangleIntersection(it.mTriangleID0, it.mTriangleID1);
		} 
	}
	groupHitPositionToGraph();

    std::cout << "Debug: find  graph  " << m_GrpahToHitPosMap.size() << std::endl;
    for (auto iter : m_GrpahToHitPosMap)
    {
        divideGraphToContour(iter.first, iter.second);
    }

    std::cout << "Debug: find contour   " << m_ContourToHitPosMap.size() << std::endl;
    for (auto iter : m_ContourToHitPosMap)
    {
        minimizeGlobalContour(iter.second);
    }

    applyOffsetToParticle();
    
    clearTmpData();
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
                tri_pair.addHitPos(hitPos.mHashCode);
                m_TriangleHitPositions.emplace(hitPos.mHashCode,  std::move(hitPos) );
                
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
                tri_pair.addHitPos(hitPos.mHashCode);
                m_TriangleHitPositions.emplace(hitPos.mHashCode, std::move(hitPos));
                
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
            tri_pair.addHitPos(hitPos.mHashCode);
            m_TriangleHitPositions.emplace(hitPos.mHashCode, std::move(hitPos));

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
                tri_pair.addHitPos(hitPos.mHashCode);
                m_TriangleHitPositions.emplace(hitPos.mHashCode, std::move(hitPos));
                
                ret = true;
            }
        }

        if (ret)
        {
            TriangleHitPoint hitPos(rerangeVertexList[0], tri_pair.mTriangleID0, tri_pair.mTriangleID1);
            tri_pair.addHitPos(hitPos.mHashCode);
            m_TriangleHitPositions.emplace(hitPos.mHashCode, std::move(hitPos));
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

int UntangleSolver::makeUniqueContourID(int graphID, int contourID)
{
    const int graphMaxLimit = 10000;
    assert(m_GrpahToHitPosMap.size() < graphMaxLimit);
    return graphID * graphMaxLimit + contourID;
}
void UntangleSolver::divideGraphToContour(int graphID, const std::set<size_t>& hitPosKeySet)
{
    int contourID = -1;
    for (auto& hpkey : hitPosKeySet)
    {
        TriangleHitPoint& hp = m_TriangleHitPositions.at(hpkey);
        
        if (hp.mContourID != -1)
            continue;

        ++contourID;
        size_t pairHpKey = findPairHitPos(hp);
        TriangleHitPoint& pairHp = m_TriangleHitPositions.at(pairHpKey);

        int glbalContourID = makeUniqueContourID( graphID, contourID);
        assert(pairHp.mContourID == -1);
        setContourIDToAdjacents(hp, glbalContourID);
        setContourIDToAdjacents(pairHp, glbalContourID);
    }

    if (contourID > 4)
    {

    }
}

void UntangleSolver::setContourIDToPair(TriangleHitPoint& hitpos, int contourID)
{
    if (hitpos.mContourID != -1)
    {
        assert(hitpos.mContourID == contourID);
        return;
    }

    hitpos.mContourID = contourID;
    m_ContourToHitPosMap[contourID].insert(hitpos.mHashCode);

    size_t pairHpKey = findPairHitPos(hitpos);
    TriangleHitPoint& pairHp = m_TriangleHitPositions.at(pairHpKey);
    setContourIDToAdjacents(pairHp, contourID);
}

void UntangleSolver::setContourIDToAdjacents(TriangleHitPoint& hitpos, int contourID)
{
    if (hitpos.mContourID != -1)
    {
        assert(hitpos.mContourID == contourID);
        return;
    }

    hitpos.mContourID = contourID;
    m_ContourToHitPosMap[contourID].insert(hitpos.mHashCode);

    if (hitpos.mType == SHARED_VERTEX)
        return;

    std::set<size_t> twinHps = findTwinsHp(hitpos);
    for (size_t hpKey : twinHps)
    {
        setContourIDToPair(m_TriangleHitPositions.at(hpKey), contourID);
    }
}


void UntangleSolver::groupHitPositionToGraph()
{
	if (!m_isGlobalScheme)
		return;


    int graphID = -1;
    for (auto& hitPosIter : m_TriangleHitPositions)
    {
        auto& hitPos = hitPosIter.second;

        if (hitPos.mGraphID != -1)
            continue;

        //a new graph
        graphID += 1;
        m_GrpahToHitPosMap.emplace(graphID, std::set<size_t>{});


        size_t pairHitPosKey = findPairHitPos(hitPos);
        TriangleHitPoint& pairHp = m_TriangleHitPositions.at(pairHitPosKey);
        
        hitPos.mPairHashcode = pairHp.mHashCode;
        pairHp.mPairHashcode = hitPos.mHashCode;

        setGraphIDTOAdjacent(pairHp, graphID);
        setGraphIDTOAdjacent(hitPos, graphID);
    }
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

size_t UntangleSolver::findPairHitPos(const TriangleHitPoint& curHp)
{
    auto tri_pair_ids = curHp.getTwoTriangles();
    TrianglePair tri_pir(tri_pair_ids.first, tri_pair_ids.second);
    auto itor = m_collisionTrianglePair.find(tri_pir);
    assert(itor != m_collisionTrianglePair.end());
    assert(itor->mHitPos.size() == 2);
    for (const auto& hpKey : itor->mHitPos)
    {
        if (curHp.mHashCode != hpKey)
            return hpKey;
    }
    //should not arrive here
    assert(false);
    return -1;
}

void UntangleSolver::setGraphIDToPair(TriangleHitPoint& src_hitPos, int graphID)
{
    if (src_hitPos.mGraphID != -1)
        return;
    src_hitPos.mGraphID = graphID;
    m_GrpahToHitPosMap[graphID].insert(src_hitPos.mHashCode);

    size_t pairHitPosKey = findPairHitPos(src_hitPos);
    TriangleHitPoint& pairHp = m_TriangleHitPositions.at(pairHitPosKey);

    pairHp.mPairHashcode = src_hitPos.mHashCode;
    src_hitPos.mPairHashcode = pairHp.mHashCode;

    setGraphIDTOAdjacent(pairHp, graphID);



}

void UntangleSolver::debugTriangleIntersection(int tri0, int tri1)
{
    auto q0 = m_particles[m_triangles[tri0].p0].mCurPosition;
    auto q1 = m_particles[m_triangles[tri0].p1].mCurPosition;
    auto q2 = m_particles[m_triangles[tri0].p2].mCurPosition;

    auto p0 = m_particles[m_triangles[tri1].p0].mCurPosition;
    auto p1 = m_particles[m_triangles[tri1].p1].mCurPosition;
    auto p2 = m_particles[m_triangles[tri1].p2].mCurPosition;

    generateMayaScriptForTriangle(q0, q1, q2, p0, p1, p2);

    testTriangleIntersect(tri0, tri1);
}

std::set<size_t> UntangleSolver::findTwinsHp(const TriangleHitPoint& src_hitPos)
{
    std::set<size_t> twinHps;
    if (src_hitPos.mType == EDGE_POINT)
    {
        Edge currentEdge(src_hitPos.mTriangleID, src_hitPos.mEdgeLocalID);
        std::list<Edge> adjacentEdges = findAdjacentTriangleEdges(currentEdge);

        for (const auto& adj_edge : adjacentEdges)
        {
            TrianglePair tri_pair(adj_edge.mTriangleID, src_hitPos.mHitTriangleID);
            auto itor = m_collisionTrianglePair.find(tri_pair);
            
            //assert(itor != m_collisionTrianglePair.end());
            if (itor == m_collisionTrianglePair.end())
            {
                debugTriangleIntersection(adj_edge.mTriangleID, src_hitPos.mHitTriangleID);
                assert(false);
            }
            
            
            const TrianglePair& adjTirPair = *itor;
            int matchIndex = -1;
            for (int i = 0, n = (int)adjTirPair.mHitPos.size(); i < n; ++i)
            {
                const auto& hitPosKey = adjTirPair.mHitPos.at(i);
                const auto& hitPos = m_TriangleHitPositions.at(hitPosKey);
                if (hitPosKey == src_hitPos.mHashCode)
                    continue; //skip itself

                if (isHitPosMatch(src_hitPos, hitPos))
                {
                    twinHps.insert(hitPosKey);
                    break;
                }
            }
        }
    }
    else
    {
        std::vector<int> adjacnetTriangles = findVertexAdjacentTriangle(src_hitPos.mVertexID);
        for (int i = 0; i < (int)adjacnetTriangles.size(); ++i)
        {
            for (int j = i + 1; j < (int)adjacnetTriangles.size(); ++j)
            {
                TrianglePair tri_pair(adjacnetTriangles[i], adjacnetTriangles[j]);
                auto itor = m_collisionTrianglePair.find(tri_pair);
                if (itor == m_collisionTrianglePair.end())
                    continue;
                const TrianglePair& adjTirPair = *itor;

                int matchIndex = -1;
                for (int i = 0, n = (int)adjTirPair.mHitPos.size(); i < n; ++i)
                {
                    const auto& hitPosKey = adjTirPair.mHitPos.at(i);
                    const auto& hitPos = m_TriangleHitPositions.at(hitPosKey);
                    if (isHitPosMatch(src_hitPos, hitPos))
                    {
                        twinHps.insert(hitPosKey);
                        break;
                    }
                }
            }
        }
    }
    return twinHps;
}

void UntangleSolver::setGraphIDTOAdjacent(TriangleHitPoint& src_hitPos, int graphID)
{
    if (src_hitPos.mGraphID != -1)
        //ring path
        return;

    src_hitPos.mGraphID = graphID;
    m_GrpahToHitPosMap[graphID].insert(src_hitPos.mHashCode);

    std::set<size_t> twinHps = findTwinsHp(src_hitPos);
    for (size_t hpKey : twinHps)
    {
        setGraphIDToPair(m_TriangleHitPositions.at(hpKey), graphID);
    }

    twinHps.insert(src_hitPos.mHashCode);

    size_t familyKey = 0;
    for (auto key : twinHps)
    {
        boost::hash_combine(familyKey, key);
    }

    for (auto key : twinHps)
    {
        TriangleHitPoint& hp = m_TriangleHitPositions.at(key);
        hp.mFaimilyHashCode = familyKey;
    }
    m_familtyHpMaps.emplace(familyKey, std::move(twinHps));
}

void UntangleSolver::minimizeLocalContour()
{
    const float h0 = 0.006;
    const float g0 = 0.006;
    for (auto& hitPosIter : m_TriangleHitPositions)
    {
        TriangleHitPoint& hitPos = hitPosIter.second;

        if (hitPos.mType == EDGE_POINT)
        {
            Vec3 G = calculateGVector(Edge(hitPos.mTriangleID, hitPos.mEdgeLocalID), hitPos.mHitTriangleID);
            Vec3 offset = G * h0 / sqrt(glm::length2(offset) + g0 * g0);

            Triangle tri0 = m_triangles[hitPos.mTriangleID];
            Triangle tri1 = m_triangles[hitPos.mHitTriangleID];

            for (auto vertex_offset_iter :
                std::initializer_list<std::pair<int, Vec3>>{
                    {tri0.p0,offset},{tri0.p1,offset},{tri0.p2,offset},
                    {tri1.p0,-offset},{tri1.p1,-offset},{tri1.p2,-offset} })
            {
                int vid = vertex_offset_iter.first;
                m_vertexAccumOffset[vid] += vertex_offset_iter.second * m_particles[vid].mInvMass;
            }
        }
        else {  /*skip share vertex case*/ }
    }
}

void UntangleSolver::minimizeGlobalContour(const std::set<size_t>& hpSets)
{
    size_t currentkey = -1;
    for (auto& key : hpSets)
    {
        if (m_TriangleHitPositions.at(key).mType != SHARED_VERTEX)
        {
            currentkey = key;
            break;
        }
    }

    assert(currentkey != -1);

    std::set<size_t> groupA;
    std::set<size_t> groupB;

    TriangleHitPoint& curHp = m_TriangleHitPositions.at(currentkey);
    size_t pairHitPosKey = findPairHitPos(curHp);
    TriangleHitPoint& pairHp = m_TriangleHitPositions.at(pairHitPosKey);

    setGroupToAdjacent(curHp, groupA, groupB);

    if (pairHp.mType != SHARED_VERTEX)
    {
        if (pairHp.mHitTriangleID == curHp.mHitTriangleID)
        {
            setGroupToAdjacent(pairHp, groupA, groupB);
        }
        else
        {
            setGroupToAdjacent(pairHp, groupB, groupA);
        }
    }



    Vec3 sum_A(0);
    Vec3 sum_B(0);

    for (auto& key : groupA)
    {
        const TriangleHitPoint& curHp = m_TriangleHitPositions.at(key);
        Vec3 G = calculateGVector(Edge(curHp.mTriangleID, curHp.mEdgeLocalID), curHp.mHitTriangleID);

        sum_A = sum_A + G;
    }

    for (auto& key : groupB)
    {
        const TriangleHitPoint& curHp = m_TriangleHitPositions.at(key);
        Vec3 G = calculateGVector(Edge(curHp.mTriangleID, curHp.mEdgeLocalID), curHp.mHitTriangleID);
        sum_B = sum_B + G;
    }

    std::vector<int> vertexFlags;
    vertexFlags.resize(m_particles.size());
    
    for (auto& iter :
        std::initializer_list<std::pair<Vec3, std::set<size_t>& >>{ {sum_A, groupA },{sum_B, groupB} })
    {
        m_vertexUpdateFlags.assign(m_particles.size(), 0);
        for (auto& key : iter.second )
        {
            Vec3 offset =  iter.first;
            if (glm::length(offset) < 1e-7)
                continue;
            //offset = glm::normalize(offset);

            const TriangleHitPoint& curHp = m_TriangleHitPositions.at(key);

            Triangle tri0 = m_triangles[curHp.mTriangleID];
            Triangle tri1 = m_triangles[curHp.mHitTriangleID];

            for (auto vertex_offset_iter :
                std::initializer_list<std::pair<int, Vec3>>{
                    {tri0.p0,offset},{tri0.p1,offset},{tri0.p2,offset},
                    {tri1.p0,-offset},{tri1.p1,-offset},{tri1.p2,-offset} })
            {
                int vid = vertex_offset_iter.first;
                if (m_vertexUpdateFlags[vid] == 0)
                {
                    m_vertexUpdateFlags[vid] = 1;
                    if (m_particles[vid].mInvMass > 0)
                    {
                        m_vertexAccumOffset[vid] += vertex_offset_iter.second * m_particles[vid].mInvMass;
                    }

                }
            }
        }
    }
}

void  UntangleSolver::applyOffsetToParticle()
{
    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        Vec3 offset = m_vertexAccumOffset[i];

        if (glm::length(offset) < 1e-7)
            continue;

        const float h0 = 0.01f;
        const float g0 = 0.06f;

        offset =  offset*h0 / sqrt(glm::length2(offset) + g0 * g0);
        m_particles[i].mCurPosition += offset;
    }
}

void UntangleSolver::setGroupToPair(TriangleHitPoint& hitPos, std::set<size_t>& edgeSideGroup, std::set<size_t>& triSideGroup)
{
    if (hitPos.mSetGroupFlag)
        return;

    edgeSideGroup.insert(hitPos.mHashCode);
    hitPos.mSetGroupFlag = true;

    size_t pairHpKey = findPairHitPos(hitPos);
    TriangleHitPoint& pairHp = m_TriangleHitPositions.at(pairHpKey);

    if (pairHp.mType == SHARED_VERTEX)
        return;

    if (pairHp.mHitTriangleID == hitPos.mHitTriangleID)
    {
        setGroupToAdjacent(pairHp, edgeSideGroup, triSideGroup);
    }
    else
    {
        setGroupToAdjacent(pairHp, triSideGroup, edgeSideGroup);
    }
}

void UntangleSolver::setGroupToAdjacent(TriangleHitPoint& hitPos, std::set<size_t>& edgeSideGroup, std::set<size_t>& triSideGroup)
{
    if (hitPos.mSetGroupFlag)
        return;

    edgeSideGroup.insert(hitPos.mHashCode);
    hitPos.mSetGroupFlag = true;

    std::set<size_t> twinHps = findTwinsHp(hitPos);
    for (size_t hpKey : twinHps)
    {
        TriangleHitPoint& adjHp = m_TriangleHitPositions.at(hpKey);
        setGroupToPair(adjHp, edgeSideGroup, triSideGroup);
    }
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
        const auto& hitPosKey = collisionTrianglePair.mHitPos.at(i);
        const auto& hitPos = m_TriangleHitPositions.at(hitPosKey);
        if (isHitPosMatch(src_hitPos,  hitPos))
        {
            matchIndex = i;
        }
    }

    assert(matchIndex == 0 || matchIndex == 1);
    size_t curHitPosKey = collisionTrianglePair.mHitPos[matchIndex];
    size_t curPairHitPosKey = collisionTrianglePair.mHitPos[1 - matchIndex];

    TriangleHitPoint& curHitPos = m_TriangleHitPositions.at(curHitPosKey);
    TriangleHitPoint& curPairHitPos = m_TriangleHitPositions.at(curPairHitPosKey);

    if (curHitPos.mGraphID != -1)
        return;
    assert(curPairHitPos.mGraphID == -1);
    curHitPos.mPairHashcode = curPairHitPosKey;
    curPairHitPos.mPairHashcode = curHitPosKey;

    curHitPos.mGraphID = graphID;
    curPairHitPos.mGraphID = graphID;

    m_GrpahToHitPosMap[graphID].insert(curHitPosKey);
    m_GrpahToHitPosMap[graphID].insert(curPairHitPosKey);

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
