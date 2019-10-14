//
// Created by Administrator on 10/12/2019.
//

#ifndef ADVANCEDGL_UNTANGLE_SOLVER_H
#define ADVANCEDGL_UNTANGLE_SOLVER_H

#include "render_type.h"
#include <map>
#include <set>
#include <vector>

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
    Edge( int triangleID , int edgeID );

    int mP0;
    int mP1;

    int mTriangleID;
    int mEdgeLocalID;

    size_t mEID;

    bool operator<( const Edge& other) const
    {
        if( mP0 < other.mP0 )
            return true;
        else if( other.mP0 < mP0 )
            return false;
        else{
            return mP1 < other.mP1;
        }
    }
};

struct TrianglePair
{
    TrianglePair( int triID0, int triID1 )
    {
        assert( triID0 != triID1 );
        mTriangleID0 = triID0 > triID1 ? triID1 : triID0;
        mTriangleID1 = triID0 < triID1 ? triID1 : triID0;
    }

    int mTriangleID0;
    int mTriangleID1;

    bool operator<( const TrianglePair& other) const
    {
        if( mTriangleID0 < other.mTriangleID0 )
            return true;
        else if( other.mTriangleID0 < mTriangleID0 )
            return false;
        else{
            return mTriangleID1 < other.mTriangleID1;
        }
    }
};

struct EdgeTrianglePair
{
    EdgeTrianglePair(const Edge& edge , int triangleID, float hit_t);
    Edge mEdge;
    int  mTriangleID;

    float mHit_t;

    bool operator<( const EdgeTrianglePair& other) const
    {
        if( mTriangleID < other.mTriangleID )
            return true;
        else if( other.mTriangleID < mTriangleID )
            return false;
        else{
            return mEdge < other.mEdge;
        }
    }

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

    Vec3 getOffset() const
    {
        return mOffset;
    }
};

class UntangleSolver {

public:
    static UntangleSolver& singleton();

    void init( std::vector<Particle>&& particles, std::vector<Triangle>&& triangles );

    std::vector<Particle>& getParticles()
    {
        return m_particles;
    }

    const std::vector<Triangle>& getTriangles() const
    {
        return m_triangles;
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

    Vec3 getTriangleNormal(int triangleID);
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
