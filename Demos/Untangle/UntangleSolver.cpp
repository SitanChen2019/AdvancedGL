//
// Created by Administrator on 10/12/2019.
//

#include "UntangleSolver.h"
#include "GeometryMath.h"
#include "boost/container_hash/hash.hpp"
#include <iostream>

UntangleSolver& UntangleSolver::singleton()
{
    static UntangleSolver s_instance;
    return s_instance;
}

void UntangleSolver::init( std::vector<Particle>&& particles, std::vector<Triangle>&& triangles )
{
    m_particles.swap( particles );
    m_triangles.swap( triangles );
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

void UntangleSolver::calculateEdgeTriangleIntersection()
{
    m_collisionEdgeTrianglePair.clear();
    for(const TrianglePair& tir_pair : m_collisionTrianglePair  )
    {
        for( int i = 0; i< 3; ++i)
        {
            Edge edge( tir_pair.mTriangleID0, i );
            float hit_t = -1;
            if( testEdgeTriangleIntersect(edge, tir_pair.mTriangleID1, hit_t) )
            {
                m_collisionEdgeTrianglePair.insert(EdgeTrianglePair(edge, tir_pair.mTriangleID1, hit_t) );
            }
        }

        for( int i = 0; i< 3; ++i)
        {
            Edge edge( tir_pair.mTriangleID1, i );
            float hit_t = -1;
            if( testEdgeTriangleIntersect(edge, tir_pair.mTriangleID0,hit_t) )
            {
                m_collisionEdgeTrianglePair.insert(EdgeTrianglePair(edge, tir_pair.mTriangleID0,hit_t) );
            }
        }
    }
}

void UntangleSolver::calculateEdgeCorrectVector()
{
    m_edgeCorrects.clear();
    for( const EdgeTrianglePair& edgeTri_pair : m_collisionEdgeTrianglePair )
    {
        const Edge& edge = edgeTri_pair.mEdge;
        int triangleID = edgeTri_pair.mTriangleID;
        Vec3 gvector = calculateGVector( edge,triangleID );
        
		addEdgeCorrectVector(Edge(edge.mTriangleID, 0), gvector);
		addEdgeCorrectVector(Edge(edge.mTriangleID, 1), gvector);
		addEdgeCorrectVector(Edge(edge.mTriangleID, 2), gvector);
		//addEdgeCorrectVector( edge , gvector );

        //Vec3 gvector_for_triangle = -gvector;
        //addEdgeCorrectVector( Edge(triangleID,0), gvector);
        //addEdgeCorrectVector( Edge(triangleID,1), gvector);
        //addEdgeCorrectVector( Edge(triangleID,2), gvector);
    }
}

void UntangleSolver::correctParticles()
{
    for( const auto& edgeCorrect : m_edgeCorrects  )
    {
        int p0 = edgeCorrect.first.mP0;
        int p1 = edgeCorrect.first.mP1;

        m_particles[p0].mCurPosition += edgeCorrect.second.getOffset() * m_particles[p0].mInvMass;
        m_particles[p1].mCurPosition += edgeCorrect.second.getOffset() * m_particles[p1].mInvMass;
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

    mEID= 0;
    boost::hash_combine(mEID,mP0);
    boost::hash_combine(mEID,mP1);

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
	if (t1.p0 == t2.p0 || t1.p0 == t2.p1 || t1.p0 == t2.p2)
		++shareVertex;
	if (t1.p1 == t2.p0 || t1.p1 == t2.p1 || t1.p1 == t2.p2)
		++shareVertex;
	if (t1.p2 == t2.p0 || t1.p2 == t2.p1 || t1.p2 == t2.p2)
		++shareVertex;
	if (shareVertex > 0)
		return false;

    return GeometryMath::isTriangleIntersect(
        m_particles[t1.p0].mCurPosition,m_particles[t1.p1].mCurPosition,m_particles[t1.p2].mCurPosition,
        m_particles[t2.p0].mCurPosition,m_particles[t2.p1].mCurPosition,m_particles[t2.p2].mCurPosition
            );
}

bool UntangleSolver::testEdgeTriangleIntersect(const Edge& edge, int triangleID, float& hit_t )
{
    Triangle& t1 = m_triangles[triangleID];

	if (edge.mP0 == 3 && edge.mP1 == 4)
	{
		std::cout << "catch" << std::endl;
	}
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

    Triangle B = m_triangles[ edge.mTriangleID ];
    Vec3 inner_point_in_B = 0.5f*m_particles[B.p0].mCurPosition
            + 0.25f*m_particles[B.p1].mCurPosition + 0.25f*m_particles[B.p2].mCurPosition;
    Vec3 tmp = (inner_point_in_B - m_particles[ edge.mP0].mCurPosition);
    if( glm::dot(tmp, R ) < 0 )
        R = -R;

    Vec3 E = m_particles[edge.mP1].mCurPosition - m_particles[edge.mP0].mCurPosition;
	E = glm::normalize(E);

	float tmp1 = glm::dot(E, R);
	float tmp2 = glm::dot(E, N);
	float tmp3 = tmp1 / tmp2;
    return R -( glm::dot(E,R)/glm::dot(E,N) ) * N;
}