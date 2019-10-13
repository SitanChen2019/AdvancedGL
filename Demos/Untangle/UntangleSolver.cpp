//
// Created by Administrator on 10/12/2019.
//

#include "UntangleSolver.h"

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
    int triangleCount = m_triangles.size();
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
        addEdgeCorrectVector( edge , gvector );

        Vec3 gvector_for_triangle = -gvector;
        addEdgeCorrectVector( Edge(triangleID,0), gvector);
        addEdgeCorrectVector( Edge(triangleID,1), gvector);
        addEdgeCorrectVector( Edge(triangleID,2), gvector);
    }
}

void UntangleSolver::correctParticles()
{
    for( const auto& edgeCorrect : m_edgeCorrects  )
    {
        int p0 = edgeCorrect.first.mP0;
        int p1 = edgeCorrect.first.mP1;

        m_particles[p0].mCurPosition += edgeCorrect.second.getOffset();
        m_particles[p1].mCurPosition += edgeCorrect.second.getOffset();
    }
}

void UntangleSolver::addEdgeCorrectVector( const Edge& edge , const Vec3 correctVector)
{
    auto it = m_edgeCorrects.find( edge );
    if( it == m_edgeCorrects.end() )
    {
        it = m_edgeCorrects.insert( std::make_pair(edge, EdgeCorrect() )).second;
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

}

EdgeTrianglePair::EdgeTrianglePair(const Edge& edge , int triangleID,float hit_t)
:mEdge(edge)
{
    mTriangleID = triangleID;
    mHit_t = hit_t;
}

bool UntangleSolver::testTriangleIntersect(int triangleID0, int triangleID1 )
{

}

bool UntangleSolver::testEdgeTriangleIntersect(const Edge& edge, int Triangle, float& hit_t )
{

}

Vec3 UntangleSolver::calculateGVector(const Edge& edge, int Triangle)
{

}