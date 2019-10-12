//
// Created by Administrator on 10/12/2019.
//

#ifndef ADVANCEDGL_UNTANGLEOSOLVER_H
#define ADVANCEDGL_UNTANGLEOSOLVER_H

#include "render_type.h"

struct Particle
{
    Vec3 mPrePosition;
    Vec3 mCurPosition;
    float mInvMass;

    int mPID;
};

struct Triangle
{
public:
    int p0;
    int p1;
    int p2;

    int mTID;
};

struct  Edge
{
    int p0;
    int p1;

    int triangleID;
    int edgeID;

    int mEID;
};

struct TrianglePair
{
    int mTriangleID0;
    int mTriangleID1;
};

struct EdgeTrianglePair
{
    Edge edge;
    int  triangleID;

    float hit_t;
};

struct EdgeCorrect
{
private:
    int mCount;
    Vec3 mOffset;

public:
    EdgeCorrect()
    {
        mCount = 0;
        mOffset = Vec3(0);
    }

    void addOffset( Vec3 offset )
    {
        mOffset += offset;
        mCount += 1;
    }

    void getOffset()
    {
        retunr mOffset;
    }
};

class UntangleSolver {

public:
    //static createUntangleSo
    UntangleSolver( std::vector<Particle>&& particles, std::vector<Triangle>&& triangles );

    std::vector<Particle>& getParticles()
    {
        return m_particles;
    }

    void preUpdate();
    void update();
    void postUpdate();

private:
    //temp data
    void findCollisionTrianglePairs();
    void calculateEdgeTriangleIntersection();
    void calculateEdgeCorrectVector();
    void correctParticles();


    bool testTriangleIntersect(int triangleID0, int triangleID1 );
    bool testEdgeTriangleIntersect(const Edge& edge, int Triangle, float& hit_t );
    Vec3 calculateGVector(const Edge& edge, int Triangle);
    void addEdgeCorrectVector( const Edge& edge , const Vec3 correctVector);
private:
    std::vector<Particle> m_particles;
    std::vector<Triangle> m_triangles;

    std::set<TrianglePair> m_collisionTrianglePair;
    std::set<EdgeTrianglePair> m_collisionEdgeTrianglePair;
    std::map<Edge,EdgeCorrect> m_edgeCorrects;

};


#endif //ADVANCEDGL_UNTANGLEOSOLVER_H
