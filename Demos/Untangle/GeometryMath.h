//
// Created by Administrator on 10/14/2019.
//

#ifndef ADVANCEDGL_GEOMETRYMATH_H
#define ADVANCEDGL_GEOMETRYMATH_H

#include "render_type.h"
#include "aabb.h"

namespace GeometryMath {
	using REAL = Vec3::value_type;

	struct Triangle
	{
	public:
		Triangle(Vec3 _p0, Vec3 _p1, Vec3 _p2);

		Vec3 v0() const { return m_v0; }
		Vec3 v1() const { return m_v1; }
		Vec3 v2() const { return m_v2; }


		bool intersect_segment(const Vec3& v0, const Vec3& v1, float& t) const;

		bool isPointInTriangle(const Vec3& intersectPt) const;

		bool intersect_traingle(const Triangle& tri) const;
        bool intersect_shared_vertex_traingle(const Vec3& p0, const Vec3& p1) const;


		AABB getBoundingBox() const;

	private:
		Vec3 m_normal;
		Vec3 m_v0;
		Vec3 m_v1;
		Vec3 m_v2;

	};

    bool isTriangleIntersect( Vec3 p00, Vec3 p01, Vec3 p02,
                              Vec3 p10, Vec3 p11, Vec3 p12);

    bool isSharedSingleVertexTriangleIntersect(Vec3 p00, Vec3 p01, Vec3 p02,
        Vec3 p0, Vec3 p1);

    bool edgeTriangleIntersect(Vec3 p00, Vec3 p01,
                               Vec3 p10, Vec3 p11, Vec3 p12,
                               float& hit_t);

}



#endif //ADVANCEDGL_GEOMETRYMATH_H
