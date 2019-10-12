//
// Created by Administrator on 10/12/2019.
//

#include "UntangleSolver.h"

UntangleSolver::UntangleSolver( std::vector<Particle>&& particles, std::vector<Triangle>&& triangles )
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

void UntangleSolver::preUpdate() {

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
            if( testEdgeTriangleIntersect(edge, tir_pair.mTriangleID1) )
            {
                m_collisionEdgeTrianglePair.insert(EdgeTrianglePair(edge, tir_pair.mTriangleID1) );
            }
        }

        for( int i = 0; i< 3; ++i)
        {
            Edge edge( tir_pair.mTriangleID1, i );
            if( testEdgeTriangleIntersect(edge, tir_pair.mTriangleID0) )
            {
                m_collisionEdgeTrianglePair.insert(EdgeTrianglePair(edge, tir_pair.mTriangleID0) );
            }
        }
    }
}

void UntangleSolver::calculateEdgeCorrectVector()
{
    m_edgeCorrects.clear();
    for( const EdgeTrianglePair& edgeTri_pair : m_collisionEdgeTrianglePair )
    {
        Edge& edge = edgeTri_pair.edge;
        int triangleID = edgeTri_pair.triangleID;
        Vec3 gvector = calculateGVector( edge,triangleID );
        addEdgeCorrectVector( edge , gvector );

        vec3 gvector_for_triangle = -gvector;
        addEdgeCorrectVector( Edge(triangleID,0), gvector);
        addEdgeCorrectVector( Edge(triangleID,1), gvector);
        addEdgeCorrectVector( Edge(triangleID,2), gvector);
    }
}

void UntangleSolver::correctParticles()
{
    for( auto )
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

void UntangleSolver::addEdgeCorrectVector( const Edge& edge , const Vec3 correctVector)
{
    auto it = m_edgeCorrects.find( edge )
    if( it == m_edgeCorrects.end() )
    {
        it = m_edgeCorrects.insert( std::make_pair(edge, EdgeCorrect) ).second;
    }
}