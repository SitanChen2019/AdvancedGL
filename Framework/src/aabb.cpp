//
// Created by Sitan Chen on 2019/9/3.
//

#include "aabb.h"
#include <algorithm>
#include <assert.h>

AABB::AABB()
{
    m_maxPt.x = -FLT_MAX;
    m_maxPt.y = -FLT_MAX;
    m_maxPt.z = -FLT_MAX;

    m_minPt.x = FLT_MAX;
    m_minPt.y = FLT_MAX;
    m_minPt.z = FLT_MAX;
}


AABB::~AABB()
{
}

AABB::AABB(const Vec3 p1, const Vec3 p2)
{
    //m_maxPt.x = std::max(p1.x, p2.x);
    //m_maxPt.y = std::max(p1.y, p2.y);
    //m_maxPt.z = std::max(p1.z, p2.z);

    //m_minPt.x = std::min(p1.x, p2.x);
    //m_minPt.y = std::min(p1.y, p2.y);
    //m_minPt.z = std::min(p1.z, p2.z);
}

AABB::AABB(const Vec3Sequence& vertices)
{
    for( auto vertex : vertices )
        merge( vertex );
}

void  AABB::merge(const Vec3Sequence& points)
{
	for (const Vec3& p : points)
		merge(p);
}

void AABB::merge(const Vec3& point)
{
    m_maxPt.x = std::max(m_maxPt.x, point.x);
    m_maxPt.y = std::max(m_maxPt.y, point.y);
    m_maxPt.z = std::max(m_maxPt.z, point.z);

    m_minPt.x = std::min(m_minPt.x, point.x);
    m_minPt.y = std::min(m_minPt.y, point.y);
    m_minPt.z = std::min(m_minPt.z, point.z);
}

void AABB::merge(const AABB& box)
{
    merge(box.m_minPt);
    merge(box.m_maxPt);
}

Vec3 AABB::center() const
{
    return (m_minPt + m_maxPt) / 2.0f;
}
Vec3 AABB::minP() const
{
    return m_minPt;
}

Vec3 AABB::maxP() const
{
    return m_maxPt;
}

float AABB::diagonalLength() const
{
    return glm::distance(m_minPt, m_maxPt);
}



unsigned AABB::subBoxId(const Vec3& point) const
{
    unsigned ret = 0;
    auto midPt = (m_maxPt + m_minPt)*0.5f;

    if (point.x >= midPt.x)
        ret |= 0x1;
    if (point.y >= midPt.y)
        ret |= 0x2;
    if (point.z >= midPt.z)
        ret |= 0x4;
    return ret;
}

AABB AABB::subBox(unsigned i) const
{
    auto midPt = (m_maxPt + m_minPt)*0.5f;
    Vec3 p0 = midPt;
    Vec3 p1 = m_minPt;

    if (i & 0x1)
    {
        p1.x = m_maxPt.x;
    }
    if (i & 0x2)
    {
        p1.y = m_maxPt.y;
    }
    if (i & 0x4)
    {
        p1.z = m_maxPt.z;
    }

    return AABB(p0, p1);
}

bool AABB::intersect_segment(const Vec3 p1, Vec3 p2)
{
    if (p1.x > m_maxPt.x && p2.x > m_maxPt.x)
        return false;
    if (p1.y > m_maxPt.y && p2.y > m_maxPt.y)
        return false;
    if (p1.z > m_maxPt.z && p2.z > m_maxPt.z)
        return false;

    if (p1.x < m_minPt.x && p2.x < m_minPt.x)
        return false;
    if (p1.y < m_minPt.y && p2.y < m_minPt.y)
        return false;
    if (p1.z < m_minPt.z && p2.z < m_minPt.z)
        return false;

    return true;

}

bool AABB::intersect_AABB(const AABB& other)
{
    if (m_maxPt.x < other.m_minPt.x || m_minPt.x > other.m_maxPt.x)
        return false;

    if (m_maxPt.y < other.m_minPt.y || m_minPt.y > other.m_maxPt.y)
        return false;

    if (m_maxPt.z < other.m_minPt.z || m_minPt.z > other.m_maxPt.z)
        return false;

    return true;
}

void AABB::generateTesellationData(AABB* pBox, Vec3Sequence& vertices, Int32Sequence& indices, Vec3Sequence& normals)
{
    vertices.clear();
    indices.clear();
    normals.clear();

    auto minPt = pBox->m_minPt;
    auto maxPt = pBox->m_maxPt;

    //ccw, bottom to top
    Vec3 v0, v1, v2, v3, v4, v5, v6, v7;


    v0 = minPt;

    v1 = v0;
    v1.z = maxPt.z;

    v2 = v1;
    v2.x = maxPt.x;

    v3 = v0;
    v3.x = maxPt.x;

    v4 = v0;
    v4.y = maxPt.y;

    v5 = v1;
    v5.y = maxPt.y;

    v6 = v2;
    v6.y = maxPt.y;

    v7 = v3;
    v7.y = maxPt.y;

    vertices.reserve(8);
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);

    //bottom face
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    //front face
    indices.push_back(5);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(6);

    //left face
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(1);

    //right face
    indices.push_back(6);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(7);

    //top face
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(7);
}

void AABB::generateTesellationData(AABB* pBox, Vec3Sequence& vertices, Int32Sequence& indices)
{
    vertices.clear();
    indices.clear();


    auto minPt = pBox->m_minPt;
    auto maxPt = pBox->m_maxPt;

    //ccw, bottom to top
    Vec3 v0, v1, v2, v3, v4, v5, v6, v7;


    v0 = minPt;

    v1 = v0;
    v1.z = maxPt.z;

    v2 = v1;
    v2.x = maxPt.x;

    v3 = v0;
    v3.x = maxPt.x;

    v4 = v0;
    v4.y = maxPt.y;

    v5 = v1;
    v5.y = maxPt.y;

    v6 = v2;
    v6.y = maxPt.y;

    v7 = v3;
    v7.y = maxPt.y;

    vertices.reserve(8);
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(3);
    indices.push_back(0);

    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(7);

    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(7);
    indices.push_back(4);
}
