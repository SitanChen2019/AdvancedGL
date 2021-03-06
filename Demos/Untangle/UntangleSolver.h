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
#include <unordered_map>
#include <boost/container_hash/hash.hpp>

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
    Edge(int triangleID, int p0, int p1);

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
		return mP0 != -1;
	}
};

enum TriangleHitPointType
{
    EDGE_POINT,
    SHARED_VERTEX,
};

struct TriangleHitPoint
{
    TriangleHitPoint(const TriangleHitPoint&) = delete;

    TriangleHitPoint(const TriangleHitPoint&& other)
        :mType( other.mType)
    {
        //memcpy_s(this, sizeof(TriangleHitPoint), &other, sizeof(TriangleHitPoint));
        memcpy(this, &other, sizeof(TriangleHitPoint));
    }

    const TriangleHitPointType mType;

    union
    {
        struct {
            int mTriangleID;
            int mEdgeLocalID;
            int mHitTriangleID;
            float m_t;
        };

        struct {
            int mVertexID;
            int mTriangleID0;
            int mTriangleID1;
        };
    };

    size_t mHashCode;
    size_t mPairHashcode;
    size_t mFaimilyHashCode;
    int    mGraphID;
    int    mContourID;
    bool   mSetGroupFlag;

    TriangleHitPoint( int vertexID, int triangleID0, int triangleID1)
        :mType( SHARED_VERTEX )
    {
        mVertexID = vertexID;
        mTriangleID0 = triangleID0 > triangleID1 ? triangleID1 : triangleID0;
        mTriangleID1 = triangleID0 > triangleID1 ? triangleID0 : triangleID1;

        mHashCode = mType;
        boost::hash_combine(mHashCode, mVertexID);
        boost::hash_combine(mHashCode, mTriangleID0);
        boost::hash_combine(mHashCode, mTriangleID1);

        mPairHashcode = -1;
        mGraphID = -1;
        mContourID = -1;
        mSetGroupFlag = false;
    }

	TriangleHitPoint( Edge edge , float t , int hitTriangleID)
		:mType(EDGE_POINT)
		,mEdgeLocalID(edge.mEdgeLocalID )
		,m_t(t)
	{
		mTriangleID = edge.mTriangleID;
        mHitTriangleID = hitTriangleID;

        mHashCode = mType;
        boost::hash_combine(mHashCode, mTriangleID);
        boost::hash_combine(mHashCode, mEdgeLocalID);
        boost::hash_combine(mHashCode, mHitTriangleID);
        boost::hash_combine(mHashCode, m_t);

        mPairHashcode = -1;
        mGraphID = -1;
        mContourID = -1;
        mSetGroupFlag = false;
	}

    std::pair<int, int> getTwoTriangles() const
    {
        if (SHARED_VERTEX == mType)
        {
            return std::make_pair(mTriangleID0, mTriangleID1);
        }
        else if (EDGE_POINT == mType)
        {
            return std::make_pair(mTriangleID, mHitTriangleID);
        }
        else
        {
            assert(false);
            return std::make_pair(-1, -1);
        }
    }


    bool operator==(const TriangleHitPoint another) const
    {
        if (mType != another.mType)
            return false;

        if (mType == SHARED_VERTEX)
        {
            return mVertexID == another.mVertexID
                && mTriangleID0 == another.mTriangleID0
                && mTriangleID1 == another.mTriangleID1;

        }
        else
        {
            return m_t == another.m_t 
                && mTriangleID == another.mTriangleID
                && mEdgeLocalID == another.mEdgeLocalID 
                && mHitTriangleID == another.mHitTriangleID;
        }
    }

    size_t hash() const
    {
        return mHashCode;
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
    TrianglePair( int triID0, int triID1)
    {
        assert( triID0 != triID1 );
        mTriangleID0 = triID0 > triID1 ? triID1 : triID0;
        mTriangleID1 = triID0 < triID1 ? triID1 : triID0;
    }

    int mTriangleID0;
    int mTriangleID1;
	std::vector<size_t> mHitPos;


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

	void addHitPos(const size_t hpKey )
	{
		mHitPos.emplace_back(hpKey);
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

enum TriangleIntersectType
{
    NO_INTERSECTION,
    NO_SHARE_INTERSECTION,
    SINGLE_SHARE_INTERSECTION,
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
	void groupHitPositionToGraph();


    int makeUniqueContourID(int graphID, int contourID);
    void divideGraphToContour(int graphID, const std::set<size_t>& hitPosSet );

    void setContourIDToPair( TriangleHitPoint& hitpos, int contourID);
    void setContourIDToAdjacents(TriangleHitPoint& hitpos, int contourID);

    void calculateEdgeCorrectVector();
    void correctParticles();

    Vec3 getTriangleNormal(int triangleID);
    TriangleIntersectType testTriangleIntersect(int triangleID0, int triangleID1 );
    bool testEdgeTriangleIntersect(const Edge& edge, int Triangle, float& hit_t );
    Vec3 calculateGVector(const Edge& edge, int Triangle);
    void addEdgeCorrectVector( const Edge& edge , const Vec3 correctVector);
	void addContourCorrectVector(int contourID, const Edge& edge, const Vec3 correctVector);

	bool testTwoTriangleEdgeCollision( TrianglePair& triPair);
	std::list<Edge> findAdjacentTriangleEdges(const Edge& edge);
    std::vector<int> findVertexAdjacentTriangle(int vertexID);
    std::set<size_t> findTwinsHp(const TriangleHitPoint& src_hitPos);

    void findContour2(int src_triangleID, int hit_triangleID, int target_triangleID, const TriangleHitPoint& src_hitPos, int graphID);
    void setGraphIDToPair(TriangleHitPoint& src_hitPos, int groupID);
    void setGraphIDTOAdjacent(TriangleHitPoint& src_hitPos, int groupID);

    int getShareVertexCount(int triangleID0, int triangleID1) const;
    std::array<int, 3> rerangeTriangleVertexListByShare(int triangleID0, int triangleID1 ) const;

    size_t findPairHitPos(const TriangleHitPoint& hitPos);
    bool isHitPosMatch(const TriangleHitPoint& hitPos1, const TriangleHitPoint& hitPos2);

    void minimizeGlobalContour(const std::set<size_t>& hpSet);
    void minimizeLocalContour();


    void setGroupToPair(TriangleHitPoint& hitPos, std::set<size_t>& edgeSideGroup,  std::set<size_t>& triSideGroup );
    void setGroupToAdjacent(TriangleHitPoint& hitPos, std::set<size_t>& edgeSideGroup, std::set<size_t>& triSideGroup);

    void applyOffsetToParticle();

    void debugTriangleIntersection(int tri0, int tri1);

    void update_local();
    void update_global();

    void clearTmpData();
private:
	bool m_isGlobalScheme = true;

    std::vector<Particle> m_particles;
    std::vector<Triangle> m_triangles;

	std::map<VertexPair, std::array<Edge, 2>> m_shareEdgeMap;

    std::set<TrianglePair> m_collisionTrianglePair;
    std::unordered_map<size_t, TriangleHitPoint> m_TriangleHitPositions;
    std::map<int, std::set<size_t>> m_GrpahToHitPosMap;
    std::map<int, std::set<size_t>> m_ContourToHitPosMap;
    std::map<size_t, std::set<size_t>> m_familtyHpMaps;

    std::vector<int> m_vertexUpdateFlags;
    std::vector<Vec3> m_vertexAccumOffset;


};


#endif //ADVANCEDGL_UNTANGLEOSOLVER_H
