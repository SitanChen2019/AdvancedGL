//
// Created by Administrator on 10/14/2019.
//

#ifndef ADVANCEDGL_GEOMETRYMATH_H
#define ADVANCEDGL_GEOMETRYMATH_H

#include "render_type.h"
#include "aabb.h"

namespace REF {

    // Three-dimensional Triangle-Triangle Overlap Test
     int tri_tri_overlap_test_3d(double p1[3], double q1[3], double r1[3],
        double p2[3], double q2[3], double r2[3]);


    // Three-dimensional Triangle-Triangle Overlap Test
    // additionaly computes the segment of intersection of the two triangles if it exists.
    // coplanar returns whether the triangles are coplanar,
    // source and target are the endpoints of the line segment of intersection
     int tri_tri_intersection_test_3d(double p1[3], double q1[3], double r1[3],
        double p2[3], double q2[3], double r2[3],
        int* coplanar,
        double source[3], double target[3],
        int i1, int i2, int i3,
        int j1, int j2, int j3,
        int& src_s, int& src_e,
        int& tgt_s, int& tgt_e,
        bool& srcOnT1, bool& tgtOnT1);


     int coplanar_tri_tri3d(double  p1[3], double  q1[3], double  r1[3],
        double  p2[3], double  q2[3], double  r2[3],
        double  N1[3], double  N2[3]);


    // Two dimensional Triangle-Triangle Overlap Test
     int tri_tri_overlap_test_2d(double p1[2], double q1[2], double r1[2],
        double p2[2], double q2[2], double r2[2]);

     bool isTriangleIntersect(Vec3 p00, Vec3 p01, Vec3 p02,
         Vec3 p10, Vec3 p11, Vec3 p12,
         int i1, int i2, int i3, 
         int j1, int j2, int j3);
}

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

    void detectIntersection3(Vec3 t0_p0, Vec3 t0_p1, Vec3 t0_p2,
        Vec3 t1_p0, Vec3 t1_p1, Vec3 t1_p2);

    bool test_edge_triangle_intersection(const Vec3& p0, const Vec3& p1, const Vec3& p2,
        const Vec3& q1, const Vec3& q2, float* t);

    bool is_point_in_triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& q);

}



#endif //ADVANCEDGL_GEOMETRYMATH_H
