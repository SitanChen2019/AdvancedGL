//
// Created by Administrator on 10/12/2019.
//

#ifndef ADVANCEDGL_UNTANGLE_SOLVER_H
#define ADVANCEDGL_UNTANGLE_SOLVER_H

#include "render_type.h"
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>



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
	Edge()
	{
		mP0 = -1;
		mP1 = -1;
		mTriangleID = -1;
		mEdgeLocalID = -1;
	}

    Edge( int triangleID , int edgeLocalID );

    int mP0;
    int mP1;

    int mTriangleID;
    int mEdgeLocalID;


    bool operator<( const Edge& other) const
    {
        if( mP0 < other.mP0 )
            return true;
        else if( other.mP0 < mP0 )
            return false;
        else{
			if (mP1 < other.mP1)
				return true;
			else if (mP1 > other.mP1)
				return false;
			else
				return mTriangleID < other.mTriangleID;
        }
    }

	bool isValid()
	{
		return mP0 > 0;
	}
};

struct TriangleHitPoint
{
	int mTriangleID;
	int mEdgeLocalID;
	float m_t;
	Vec3 mHitCoordinate;

	TriangleHitPoint( Edge edge , float t)
		:mHitCoordinate(Vec3(0))
		,mEdgeLocalID(edge.mEdgeLocalID )
		,m_t(t)
	{
		mTriangleID = edge.mTriangleID;
		if (edge.mEdgeLocalID == 0)
		{
			mHitCoordinate = Vec3(t, 1 - t, 0);
		}
		else if (edge.mEdgeLocalID == 1)
		{
			mHitCoordinate = Vec3(0, t, 1 - t);
		}
		else if (edge.mEdgeLocalID == 2)
		{
			mHitCoordinate = Vec3(1 - t, 0, t);
		}
		else
		{
			assert(false);
		}
	}
};


struct VertexPair
{
	int mP0;
	int mP1;

	VertexPair( int p0, int p1)
	{
		mP0 = p0 < p1 ? p0 : p1;
		mP1 = p0 < p1 ? p1 : p0;
	}

	bool operator<(const VertexPair& other) const
	{
		if (mP0 < other.mP0)
			return true;
		else if (other.mP0 < mP0)
			return false;
		else {
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
	std::vector<TriangleHitPoint> mHitPos;

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

	void addHitPos(Edge edge , float t)
	{
        TriangleHitPoint tmp(edge,t);
        if (t == 0 || t == 1)
        {
            for (auto& data : mHitPos)
            {
                if (data.mHitCoordinate == tmp.mHitCoordinate)
                    return;
            }
        }
		mHitPos.emplace_back(edge, t);
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
		//std::cout << glm::to_string(offset) << " " << glm::to_string(mOffset) << std::endl;
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

	bool isGlobalScheme()
	{
		return m_isGlobalScheme;
	}

	void setGlobalScheme(bool bValue)
	{
		m_isGlobalScheme = bValue;
	}

	void updateParticleInvMass(size_t beginIdx, size_t endIdx, float value)
	{
		for (size_t i = beginIdx; i < endIdx; ++i)
		{
			if (i < m_particles.size())
				m_particles[i].mInvMass = value;
		}
	}

private:
    //temp data
    void findCollisionTrianglePairs();
    void calculateEdgeTriangleIntersection();
	void groupEdgeToContour();
    void calculateEdgeCorrectVector();
    void correctParticles();

    Vec3 getTriangleNormal(int triangleID);
    bool testTriangleIntersect(int triangleID0, int triangleID1 );
    bool testEdgeTriangleIntersect(const Edge& edge, int Triangle, float& hit_t );
    Vec3 calculateGVector(const Edge& edge, int Triangle);
    void addEdgeCorrectVector( const Edge& edge , const Vec3 correctVector);
	void addContourCorrectVector(int contourID, const Edge& edge, const Vec3 correctVector);

	bool testTwoTriangleEdgeCollision( TrianglePair& triPair);
	Edge findAdjacentTriangleEdge(const Edge& edge);

	void findContour(int edgeTraignleID, int triangleID, Edge currentEdge, int contourId);

private:
	bool m_isGlobalScheme = true;

    std::vector<Particle> m_particles;
    std::vector<Triangle> m_triangles;

	std::map<VertexPair, std::array<Edge, 2>> m_shareEdgeMap;

    std::set<TrianglePair> m_collisionTrianglePair;
    std::set<EdgeTrianglePair> m_collisionEdgeTrianglePair;


    std::map<Edge,EdgeCorrect> m_edgeCorrects;

	std::map<EdgeTrianglePair, int> m_edgeToContour;
	std::map<int, EdgeCorrect> m_contourCorrect;
	std::map<int, std::set<int>> m_contourToVertexMap;

};


#endif //ADVANCEDGL_UNTANGLEOSOLVER_H
