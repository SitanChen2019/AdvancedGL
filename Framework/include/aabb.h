//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_AABB_H
#define ADVANCEDGL_AABB_H

#include "render_type.h"



class AABB
{
public:
    AABB();
    AABB(const Vec3 p1, const Vec3 p2);
    AABB( const Vec3Sequence& points );
    ~AABB();

    void merge(const Vec3& point);
    void merge(const AABB& box);
	void merge(const Vec3Sequence& points);

    Vec3 center() const;
    Vec3 minP() const;
    Vec3 maxP() const;
    float diagonalLength() const;
    bool intersect_segment(const Vec3 p1, Vec3 p2);
    bool intersect_AABB(const AABB& other);

    unsigned subBoxId(const Vec3& point) const;
    AABB subBox(unsigned i) const;

    static void generateTesellationData(AABB* pBox, Vec3Sequence& vertices, Int32Sequence& indices, Vec3Sequence& normals ); //triangles
    static void generateTesellationData(AABB* pBox, Vec3Sequence& vertices, Int32Sequence& indices); //lines

    static void test();
private:
    Vec3 m_minPt = Vec3{};
    Vec3 m_maxPt = Vec3{};
};




#endif //ADVANCEDGL_AABB_H
