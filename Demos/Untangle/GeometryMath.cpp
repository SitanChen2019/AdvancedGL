//
// Created by Administrator on 10/14/2019.
//

#include "GeometryMath.h"
#include <iostream>

namespace GeometryMath {


	bool isStrictSameSignal(REAL value1, REAL value2)
	{
		if (value1 > 0 && value2 > 0)
			return true;
		if (value1 < 0 && value2 < 0)
			return true;
		return false;
	}

	bool isSameSignal(REAL value1, REAL value2)
	{
		return value1 * value2 >= 0;
	}

	bool isDifferentSignal(REAL value1, REAL value2)
	{
		return value1 * value2 <= 0;
	}

	bool isStrictDifferentSignal(REAL value1, REAL value2)
	{
		return value1 * value2 < 0;
	}

	//return 0 means same-line
	//otherwide return 12, 13, 23
	int isTwoVectorOnSameLine(const Vec3& V0, const Vec3& V1)
	{
		/* v0.x    v0.y      v0.z
		  ------ = ------ = ----- = k
		   v1.x    v1.y      v1.z
		   to avoid dividing 0 error, we use the following formula:
		*/

		if (fabs(V0.x * V1.y - V1.x * V0.y) > 1e-7 )
			return 12;

		if (fabs(V0.x * V1.z - V1.x * V0.z) > 1e-7 )
			return 13;

		if (fabs(V0.y * V1.z - V1.y * V0.z) > 1e-7)
			return 23;

		return 0;

	}


	Triangle::Triangle(Vec3 _p0, Vec3 _p1, Vec3 _p2)
	{
		m_v0 = _p0;
		m_v1 = _p1;
		m_v2 = _p2;

		m_normal = glm::normalize(glm::cross(_p1 - _p0, _p2 - _p0));
	}

	bool Triangle::intersect_segment(const Vec3& p0, const Vec3& p1, REAL& t) const
	{	
		if (p0 == p1)
		{
			if (glm::dot(m_normal, p0 - m_v0) == 0)
			{
				t = 0.0f;
				return isPointInTriangle(p0);
			}
			return false;
		}

		/*
		* first calculate if segment intersect with triangle plane
		*/
		auto vV0P0 = p0 - m_v0;
		auto vV0P1 = p1 - m_v0;

		auto proj_V0P0 = glm::dot(vV0P0, m_normal);
		auto proj_V0P1 = glm::dot(vV0P1, m_normal);


		if (isStrictSameSignal(proj_V0P0, proj_V0P1))
			//two points are on the same side on triangle plane.
			return false;

		if (proj_V0P0 == 0 && proj_V0P1 == 0)
		{
			return false;
			//this is the case, the segment is just on the plane

			//TODO handle this case
			//assert(false);
			//return true;
		}
		else
		{
			//the following is safe if p0 or p1 is on the plane.
			t = abs(proj_V0P0) / (abs(proj_V0P0) + abs(proj_V0P1));
			Vec3 intersectPt = p0 + (p1 - p0) * t;
			return isPointInTriangle(intersectPt);
		}

	}


	//the intersectPt must be on the triangle plane
	bool Triangle::isPointInTriangle(const Vec3& intersectPt) const
	{
        if (glm::dot(glm::cross(m_v1 - intersectPt, m_v2 - intersectPt), m_normal) < 0)
            return false;
        if (glm::dot(glm::cross(m_v2 - intersectPt, m_v0 - intersectPt), m_normal) < 0)
            return false;
        if (glm::dot(glm::cross(m_v0 - intersectPt, m_v1 - intersectPt), m_normal) < 0)
            return false;
        return true;
	}

	//void Triangle::test(Vec3 t1, Vec3 t2, Vec3 t3, Vec3 p1, Vec3 p2)
	//{
	//	Triangle tri{ t1,t2,t3 };

	//	float t;
	//	if (tri.intersect_segment(p1, p2, t))
	//	{
	//		Vec3 hitPos = p1 + t * (p2 - p1);
	//		std::cout << "hit:" << hitPos.x << " " << hitPos.y << " " << hitPos.z << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << "not hit:" << std::endl;
	//	}
	//	return;
	//}


    //p0 and p1 are the tow vertices that are not shared
    bool Triangle::intersect_shared_vertex_traingle( const Vec3& p0, const Vec3& p1) const
    {
        glm::vec3 r0 = p0 - m_v2;
        glm::vec3 r1 = p1 - m_v2;

        glm::vec3 e0 = m_v0 - m_v2;
        glm::vec3 e1 = m_v1 - m_v2;
        glm::vec3 n = glm::cross(e0, e1);

        float D0 = glm::dot(r0, n);
		float D1 = glm::dot(r1, n);

        if (D0 * D1 > 0)
            return false;

        //D0 and D1 must be have different signal to ensure there is a hit point on the plane
        float beta01 = D1 / (D1 - D0);
        glm::vec3  t0 = beta01 * r0 + (1 - beta01) * r1;
  
        //only if t0 is in (+,-) then there is a intersection
        glm::vec3 e0t0 = glm::cross(e0, t0);
        glm::vec3 e1t0 = glm::cross(e1, t0);
 
        if (glm::dot(e0t0, n) >= 0
            && glm::dot(e1t0, n) <= 0)
        {
            return true;
        }

        return false;
    }

	bool Triangle::intersect_traingle(const Triangle& tri) const
	{
		//return getBoundingBox().intersect_AABB(tri.getBoundingBox());

		//a faster triangle-to-triangle intersection test algorithm.pdf

		//stage 1
		glm::vec3 r0 = tri.m_v0 - m_v2;
		glm::vec3 r1 = tri.m_v1 - m_v2;
		glm::vec3 r2 = tri.m_v2 - m_v2;


		glm::vec3 e0 = m_v0 - m_v2;
		glm::vec3 e1 = m_v1 - m_v2;
		glm::vec3 n = glm::cross(e0, e1);

		auto testT0T1crossE0E1 = [e0,e1,n](glm::vec3 t0, glm::vec3 t1)
		{
			int stage2_key = 0;
			glm::vec3 e0t0 = glm::cross(e0, t0);
			glm::vec3 e1t0 = glm::cross(e1, t0);
			glm::vec3 e0t1 = glm::cross(e0, t1);
			glm::vec3 e1t1 = glm::cross(e1, t1);

			for (auto& vect : std::array<glm::vec3, 4>{e0t0, e1t0, e0t1, e1t1})
			{
				stage2_key = (stage2_key << 1) + ( glm::dot(vect, n) > 0 ? 1 : 0 );
			}

			const int  result_matrix[16] = {
				-1,-1,1,2,
				-1,-1,4,-1,
				1,4,3,1,
				2,-1,1,-1
			};
			int stage2_case = result_matrix[stage2_key];

			if (stage2_case == -1)
				return false;
			else if (stage2_case == 1)
			{
				glm::vec3 p_in = t0;
				glm::vec3 p_out = t1;
				if (stage2_key == 2 || stage2_key == 14)
				{
					p_in = t1;
					p_out = t0;
				}

				glm::vec3 feature_edge = e0;
				float signal = -1.0f;
				if (stage2_key == 11 || stage2_key == 14)
				{
					feature_edge = e1;
					signal = 1.0f;
				}


				//check p_in if in the triangle, if yes,must intersection
				//(p_in - e0)x(p_in - e1) equidirectional to n
				if (glm::dot(glm::cross(p_in - e0, p_in - e1), n) >= 0)
				{
					return true;
				}
				else if (glm::dot(glm::cross(p_out - feature_edge, p_in - feature_edge), signal * n) >= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if (stage2_case == 2)
			{
				glm::vec3 p_3 = t0;
				glm::vec3 p_1 = t1;

				if (stage2_key == 12)
				{
					p_3 = t1;
					p_1 = t0;
				}

				if (glm::dot(glm::cross(p_3, p_1), n) >= 0)
				{
					if (glm::dot(glm::cross(p_1 - e0, p_3 - e0), n) >= 0)
					{
						return true;
					}
					else if (glm::dot(glm::cross(p_1 - e1, p_3 - e1), n) >= 0)
					{
						return true;
					}
					else
						return false;
				}
				else
				{
					return false;
				}
			}
			else if (stage2_case == 3)
			{
				//test if t0 or t1 is in the triangle
				if (glm::dot(glm::cross(e1 - e0, t0 - e0), n) >= 0)
				{
					return true;
				}
				else if (glm::dot(glm::cross(e1 - e0, t1 - e0), n) >= 0)
				{
					return true;
				}
				else
					return false;
			}
			else if (stage2_case == 4)
			{
				glm::vec3 p_in = t0;
				glm::vec3 p_out = t1;
				if (stage2_key == 6)
				{
					p_in = t1;
					p_out = t0;
				}

				if (glm::dot(glm::cross(p_in, p_out), n) >= 0)
				{
					if (glm::dot(glm::cross(p_out - e1, p_in - e1), n) >= 0)
					{
						return true;
					}
					else
						return false;
				}
				else
				{
					if (glm::dot(glm::cross(p_in - e0, p_out - e0), n) >= 0)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
			else
			{
				assert(false);
				return false;
			}

			return false;
		};

		
		float D0 = glm::dot(r0, n);
		float D1 = glm::dot(r1, n);
		float D2 = glm::dot(r2, n);

		int positive_num = 0;
		int negitive_num = 0;
		int near_zero_num = 0;

		int sign_key = 0;
		int near_zero_value = 0;
		
		std::array<float, 3> d_array = { D2, D1, D0 };
		float E = std::numeric_limits<float>::epsilon();
		for (int i = 0; i < d_array.size(); ++i)
		{
			float value = d_array[i];
			if (value > E)
			{
				sign_key |= (1 << i);

			}
			else if (value < -E)
			{
				//do nothing
			}
			else
			{
				near_zero_num |= (1 << i);
			}
		}

		glm::vec3 t0, t1;
		if (near_zero_num != 0)
		{
            //auto normal_n = glm::normalize(n);
            //auto projectOnPlane = [normal_n](glm::vec3 p)
            //{
            //    if (std::isnan(normal_n.x))
            //        return p;
            //    return p - glm::dot(p, normal_n) * normal_n;
            //};

			if (near_zero_num == 7)
			{
				//test if two coplanar triangle intersections
				//return testT0T1crossE0E1(r0, r1) || testT0T1crossE0E1(r0, r2) || testT0T1crossE0E1(r1, r2);
                return false; //we ignore the hit in this case
			}
			else if (near_zero_num == 6 || near_zero_num == 5 || near_zero_num == 3)
			{
				if (near_zero_num == 6) // D0 D1 is zero
				{
					t0 = r0;
					t1 = r1;
				}
				else if (near_zero_num == 5) // D0 D2 is zero
				{
					t0 = r0;
					t1 = r2;
				}
				else if ( near_zero_num == 3) //D1 D2 is zero
				{
					t0 = r1;
					t1 = r2;
				}
			}
			else if (near_zero_num == 4 || near_zero_num == 2 || near_zero_num == 1) 
			{
				//one is on the plane, the other two d should be different sign

				if (near_zero_num == 4) //D0 is zero
				{
                    if (sign_key == 1 || sign_key == 2 )
                    {
                        float beta12 = D2 / (D2 - D1);
                        t0 = beta12 * r1 + (1 - beta12) * r2;
                        t1 = r0;
                    }
                    else
                    {
                        //test if r0 is in the plane
                        return Triangle(Vec3(0), e0, e1).isPointInTriangle(r0);
                    }


				}
				else if (near_zero_num == 2) //D1 is zero
				{
                    if (sign_key == 4 || sign_key == 1)
                    {
                        float beta02 = D2 / (D2 - D0);
                        t0 = beta02 * r0 + (1 - beta02) * r2;
                        t1 = r1;
                    }
                    else
                    {
                        return Triangle(Vec3(0), e0, e1).isPointInTriangle(r1);
                    }
				}
				else if (near_zero_num == 1) //D2 is zero
				{
                    if (sign_key == 4 || sign_key == 2)
                    {
                        float beta01 = D1 / (D1 - D0);
                        t0 = beta01 * r0 + (1 - beta01) * r1;
                        t1 = r2;
                    }
                    else
                    {
                        return Triangle(Vec3(0), e0, e1).isPointInTriangle(r2);
                    }
				}
			}
		}
		else
		{
			switch (sign_key)
			{
				case 0:  //---
				case 7:  //+++
					return false;
				case 1:  //++-
				case 6:  //--+
				{
					//02,12
					float beta02 = D2 / (D2 - D0);
					float beta12 = D2 / (D2 - D1);
					t0 = beta02 * r0 + (1 - beta02) * r2;
					t1 = beta12 * r1 + (1 - beta12) * r2;
				}
					break;
				case 2:  //-+-
				case 5:  //+-+
				{
					//12,01
					float beta12 = D2 / (D2 - D1);
					float beta01 = D1 / (D1 - D0);
					t0 = beta12 * r1 + (1 - beta12) * r2;
					t1 = beta01 * r0 + (1 - beta01) * r1;
				}
					break;
				case 3: //-++
				case 4: //+--
				{
					//01,02
					float beta01 = D1 / (D1 - D0);
					float beta02 = D2 / (D2 - D0);
					t0 = beta01 * r0 + (1 - beta01) * r1;
					t1 = beta02 * r0 + (1 - beta02) * r2;
				}
					break;
			default:
				break;
			}
		}
		return testT0T1crossE0E1(t0, t1);

	}



	AABB Triangle::getBoundingBox() const
	{
		AABB box(m_v0, m_v1);
		box.merge(m_v2);
		return box;
	}


    bool isTriangleIntersect( Vec3 p00, Vec3 p01, Vec3 p02,
                              Vec3 p10, Vec3 p11, Vec3 p12)
    {
        float mulitply = 1.f;
        Triangle t0(p00*mulitply, p01*mulitply, p02*mulitply);
        Triangle t1(p10*mulitply, p11*mulitply, p12*mulitply);
        return t0.intersect_traingle(t1);
    }


    bool isSharedSingleVertexTriangleIntersect(
        Vec3 p00, Vec3 p01, Vec3 p02,
        Vec3 p0, Vec3 p1)
    {
        //p00 is the shared vertex
        Triangle t0(p01, p02, p00);
        return t0.intersect_shared_vertex_traingle(p0,p1);
    }

    bool edgeTriangleIntersect(Vec3 p00, Vec3 p01,
                               Vec3 p10, Vec3 p11, Vec3 p12,
                               float& hit_t)
    {

        Triangle t(p10, p11, p12);
        return  t.intersect_segment(p00, p01, hit_t);;
    }

    bool is_point_in_triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& q)
    {
        Vec3 normal = cross(p1 - p0, p2 - p0);
        normal = normalize(normal);

        if (dot(cross(p1 - q, p2 - q), normal) < 0)
            return false;
        if (dot(cross(p2 - q, p0 - q), normal) < 0)
            return false;
        if (dot(cross(p0 - q, p1 - q), normal) < 0)
            return false;
        return true;
    }

    bool test_edge_triangle_intersection(const Vec3& p0, const Vec3& p1, const Vec3& p2,
        const Vec3& q1, const Vec3& q2, float* t)
    {

        Vec3 r0 = q1 - p0;
        Vec3 r1 = q2 - p0;

        Vec3 e0 = p1 - p0;
        Vec3 e1 = p2 - p0;
        Vec3 n = cross(e0, e1);
        n = normalize(n);

        float D0 = dot(r0, n);
        float D1 = dot(r1, n);

        //D0 and D1 must be have different signal to ensure there is a hit point on the plane
        if (D0 * D1 > 0)
            return false;

        if (fabs(D0) < FLT_EPSILON && fabs(D1) < FLT_EPSILON)
            //this is the case, the segment is just on the plane
            return false;

        float beta01 = D1 / (D1 - D0);
        Vec3  t0 = beta01 * r0 + (1 - beta01) * r1;

        *t = 1 - beta01;

        return is_point_in_triangle(Vec3(0, 0, 0), e0, e1, t0);
    }

    void detectIntersection3(Vec3 t0_p0, Vec3 t0_p1, Vec3 t0_p2,
        Vec3 t1_p0, Vec3 t1_p1, Vec3 t1_p2)
    {
        int intersectPtIndex = 0;

        //Vec3 array[6] = {
        //                Vec3(0.055987, 1.353311, 0.101129),
        //                Vec3(0.078218, 1.352396, 0.101684),
        //                Vec3(0.073629, 1.361798, 0.077059),
        //                Vec3(0.076033, 1.358053, 0.088566),
        //                Vec3(0.074344, 1.354258, 0.091342),
        //                Vec3(0.077621, 1.355455, 0.089985)
        //};

        //Vec3 t0_p0 = array[0];
        //Vec3 t0_p1 = array[1];
        //Vec3 t0_p2 = array[2];

        //Vec3 t1_p0 = array[3];
        //Vec3 t1_p1 = array[4];
        //Vec3 t1_p2 = array[5];

        float t;
        if (intersectPtIndex < 2 && test_edge_triangle_intersection(t0_p0, t0_p1, t0_p2, t1_p0, t1_p1, &t))
        {
            printf("branch 1 \n");
            intersectPtIndex++;
        }
        if (intersectPtIndex < 2 && test_edge_triangle_intersection(t0_p0, t0_p1, t0_p2, t1_p1, t1_p2, &t))
        {
            printf("branch 2 \n");
            intersectPtIndex++;
        }
        if (intersectPtIndex < 2 && test_edge_triangle_intersection(t0_p0, t0_p1, t0_p2, t1_p2, t1_p0, &t))
        {
            printf("branch 3 \n");
            intersectPtIndex++;
        }
        if (intersectPtIndex < 2 && test_edge_triangle_intersection(t1_p0, t1_p1, t1_p2, t0_p0, t0_p1, &t))
        {
            printf("branch 4 \n");
            intersectPtIndex++;
        }
        if (intersectPtIndex < 2 && test_edge_triangle_intersection(t1_p0, t1_p1, t1_p2, t0_p1, t0_p2, &t))
        {
            printf("branch 5 \n");
            intersectPtIndex++;
        }
        if (intersectPtIndex < 2 && test_edge_triangle_intersection(t1_p0, t1_p1, t1_p2, t0_p2, t0_p0, &t))
        {
            printf("branch 6 \n");
            intersectPtIndex++;
        }

        if (intersectPtIndex != 2)
        {
            printf("Error test intersection. \n");
        }
    }


}


namespace REF {
#define DIM 3

    /* some 3D macros */

#define CROSS(dest,v1,v2)                       \
               dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
               dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
               dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])



#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; \
                        dest[1]=v1[1]-v2[1]; \
                        dest[2]=v1[2]-v2[2];



#define SCALAR(dest,alpha,v) dest[0] = alpha * v[0]; \
                             dest[1] = alpha * v[1]; \
                             dest[2] = alpha * v[2];

#define COPY(dest, src) memcpy(dest, src, sizeof(double) * DIM);

#define CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) {\
  SUB(v1,p2,q1)\
  SUB(v2,p1,q1)\
  CROSS(N1,v1,v2)\
  SUB(v1,q2,q1)\
  if (DOT(v1,N1) > 0.0f) return 0;\
  SUB(v1,p2,p1)\
  SUB(v2,r1,p1)\
  CROSS(N1,v1,v2)\
  SUB(v1,r2,p1) \
  if (DOT(v1,N1) > 0.0f) return 0;\
  else return 1; }



/* Permutation in a canonical form of T2's vertices */

#define TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
  if (dp2 > 0.0f) { \
     if (dq2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2) \
     else if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
     else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) }\
  else if (dp2 < 0.0f) { \
    if (dq2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
    else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
    else CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
  } else { \
    if (dq2 < 0.0f) { \
      if (dr2 >= 0.0f)  CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
      else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2)\
    } \
    else if (dq2 > 0.0f) { \
      if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
      else  CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
    } \
    else  { \
      if (dr2 > 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
      else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2)\
      else return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1,N2);\
     }}}



/*
*
*  Three-dimensional Triangle-Triangle Overlap Test
*
*/


    int tri_tri_overlap_test_3d(double p1[3], double q1[3], double r1[3],

        double p2[3], double q2[3], double r2[3])
    {
        double dp1, dq1, dr1, dp2, dq2, dr2;
        double v1[3], v2[3];
        double N1[3], N2[3];

        /* Compute distance signs  of p1, q1 and r1 to the plane of
        triangle(p2,q2,r2) */


        SUB(v1, p2, r2)
            SUB(v2, q2, r2)
            CROSS(N2, v1, v2)

            SUB(v1, p1, r2)
            dp1 = DOT(v1, N2);
        SUB(v1, q1, r2)
            dq1 = DOT(v1, N2);
        SUB(v1, r1, r2)
            dr1 = DOT(v1, N2);

        if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f))  return 0;

        /* Compute distance signs  of p2, q2 and r2 to the plane of
        triangle(p1,q1,r1) */


        SUB(v1, q1, p1)
            SUB(v2, r1, p1)
            CROSS(N1, v1, v2)

            SUB(v1, p2, r1)
            dp2 = DOT(v1, N1);
        SUB(v1, q2, r1)
            dq2 = DOT(v1, N1);
        SUB(v1, r2, r1)
            dr2 = DOT(v1, N1);

        if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) return 0;

        /* Permutation in a canonical form of T1's vertices */


        if (dp1 > 0.0f) {
            if (dq1 > 0.0f) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
            else if (dr1 > 0.0f) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
            else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
        }
        else if (dp1 < 0.0f) {
            if (dq1 < 0.0f) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
            else if (dr1 < 0.0f) TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
            else TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
        }
        else {
            if (dq1 < 0.0f) {
                if (dr1 >= 0.0f) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
                else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
            }
            else if (dq1 > 0.0f) {
                if (dr1 > 0.0f) TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
                else TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
            }
            else {
                if (dr1 > 0.0f) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
                else if (dr1 < 0.0f) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
                else return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
            }
        }
    };



    int coplanar_tri_tri3d(double p1[3], double q1[3], double r1[3],
        double p2[3], double q2[3], double r2[3],
        double normal_1[3], double normal_2[3]) {

        double P1[2], Q1[2], R1[2];
        double P2[2], Q2[2], R2[2];

        double n_x, n_y, n_z;

        n_x = ((normal_1[0] < 0) ? -normal_1[0] : normal_1[0]);
        n_y = ((normal_1[1] < 0) ? -normal_1[1] : normal_1[1]);
        n_z = ((normal_1[2] < 0) ? -normal_1[2] : normal_1[2]);


        /* Projection of the triangles in 3D onto 2D such that the area of
        the projection is maximized. */


        if ((n_x > n_z) && (n_x >= n_y)) {
            // Project onto plane YZ

            P1[0] = q1[2]; P1[1] = q1[1];
            Q1[0] = p1[2]; Q1[1] = p1[1];
            R1[0] = r1[2]; R1[1] = r1[1];

            P2[0] = q2[2]; P2[1] = q2[1];
            Q2[0] = p2[2]; Q2[1] = p2[1];
            R2[0] = r2[2]; R2[1] = r2[1];

        }
        else if ((n_y > n_z) && (n_y >= n_x)) {
            // Project onto plane XZ

            P1[0] = q1[0]; P1[1] = q1[2];
            Q1[0] = p1[0]; Q1[1] = p1[2];
            R1[0] = r1[0]; R1[1] = r1[2];

            P2[0] = q2[0]; P2[1] = q2[2];
            Q2[0] = p2[0]; Q2[1] = p2[2];
            R2[0] = r2[0]; R2[1] = r2[2];

        }
        else {
            // Project onto plane XY

            P1[0] = p1[0]; P1[1] = p1[1];
            Q1[0] = q1[0]; Q1[1] = q1[1];
            R1[0] = r1[0]; R1[1] = r1[1];

            P2[0] = p2[0]; P2[1] = p2[1];
            Q2[0] = q2[0]; Q2[1] = q2[1];
            R2[0] = r2[0]; R2[1] = r2[1];
        }

        return tri_tri_overlap_test_2d(P1, Q1, R1, P2, Q2, R2);

    };



    /*
    *
    *  Three-dimensional Triangle-Triangle Intersection
    *
    */

    /*
    This macro is called when the triangles surely intersect
    It constructs the segment of intersection of the two triangles
    if they are not coplanar.
    */

#define CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2,i1,i2,i3,j1,j2,j3) { \
  SUB(v1,q1,p1) \
  SUB(v2,r2,p1) \
  CROSS(N,v1,v2) \
  SUB(v,p2,p1) \
  if (DOT(v,N) > 0.0f) {\
    SUB(v1,r1,p1) \
    CROSS(N,v1,v2) \
    if (DOT(v,N) <= 0.0f) { \
      SUB(v2,q2,p1) \
      CROSS(N,v1,v2) \
      if (DOT(v,N) > 0.0f) { \
  SUB(v1,p1,p2) \
  SUB(v2,p1,r1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
 /* src between p1 and r1 */\
  SUB(source,p1,v1) \
  src_s = i1;\
  src_e = i3;\
  srcOnT1 = true;\
  SUB(v1,p2,p1) \
  SUB(v2,p2,r2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
/* tgt between p2 and r2 */\
  SUB(target,p2,v1) \
  tgt_s = j1;\
  tgt_e = j3;\
  tgtOnT1 = false;\
  return 1; \
      } else { \
  SUB(v1,p2,p1) \
  SUB(v2,p2,q2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
/* src between p2 and q2 */\
  SUB(source,p2,v1) \
  src_s = j1;\
  src_e = j2;\
  srcOnT1 = false;\
  SUB(v1,p2,p1) \
  SUB(v2,p2,r2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
/* tgt between p2 and r2 */\
  SUB(target,p2,v1) \
  tgt_s = j1;\
  tgt_e = j3;\
  tgtOnT1 = false;\
  return 1; \
      } \
    } else { \
      return 0; \
    } \
  } else { \
    SUB(v2,q2,p1) \
    CROSS(N,v1,v2) \
    if (DOT(v,N) < 0.0f) { \
      return 0; \
    } else { \
      SUB(v1,r1,p1) \
      CROSS(N,v1,v2) \
      if (DOT(v,N) >= 0.0f) { \
  SUB(v1,p1,p2) \
  SUB(v2,p1,r1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
/* src between p1 and r1 */\
  SUB(source,p1,v1) \
  SUB(v1,p1,p2) \
  SUB(v2,p1,q1) \
  src_s = i1;\
  src_e = i3;\
  srcOnT1 = true;\
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
/* tgt between p1 and q1 */\
  SUB(target,p1,v1) \
  tgt_s = i1;\
  tgt_e = i2;\
  tgtOnT1 = true;\
  return 1; \
      } else { \
  SUB(v1,p2,p1) \
  SUB(v2,p2,q2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
/* src between p2 and q2 */\
  SUB(source,p2,v1) \
  src_s = j1;\
  src_e = j2;\
  srcOnT1 = false;\
  SUB(v1,p1,p2) \
  SUB(v2,p1,q1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
/* src between p1 and q1 */\
  SUB(target,p1,v1) \
  tgt_s = i1;\
  tgt_e = i2;\
  tgtOnT1 = true;\
  return 1; \
      }}}}



#define TRI_TRI_INTER_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2,i1,i2,i3,j1,j2,j3) { \
  if (dp2 > 0.0f) { \
     if (dq2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2,i1,i3,i2,j3,j1,j2) \
     else if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2,i1,i3,i2,j2,j3,j1)\
     else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2,i1,i2,i3,j1,j2,j3) }\
  else if (dp2 < 0.0f) { \
    if (dq2 < 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2,i1,i2,i3,j3,j1,j2)\
    else if (dr2 < 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2,i1,i2,i3,j2,j3,j1)\
    else CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2,i1,i3,i2,j1,j2,j3)\
  } else { \
    if (dq2 < 0.0f) { \
      if (dr2 >= 0.0f)  CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2,i1,i3,i2,j1,j3,j2)\
      else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2,i1,i2,i3,j1,j2,j3)\
    } \
    else if (dq2 > 0.0f) { \
      if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2,i1,i3,i2,j1,j2,j3)\
      else  CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2,i1,i2,i3,j2,j3,j1)\
    } \
    else  { \
      if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2,i1,i2,i3,j3,j1,j2)\
      else if (dr2 < 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2,i1,i3,i2,j3,j1,j2)\
      else { \
        *coplanar = 1; \
  return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1,N2);\
     } \
  }} }


    /*
    The following version computes the segment of intersection of the
    two triangles if it exists.
    coplanar returns whether the triangles are coplanar
    source and target are the endpoints of the line segment of intersection
    */

    int tri_tri_intersection_test_3d(double p1[3], double q1[3], double r1[3],
        double p2[3], double q2[3], double r2[3],
        int* coplanar,
        double source[3], double target[3],
        int i1, int i2, int i3,
        int j1, int j2, int j3,
        int& src_s, int& src_e,
        int& tgt_s, int& tgt_e,
        bool& srcOnT1, bool& tgtOnT1)
    {
        double dp1, dq1, dr1, dp2, dq2, dr2;
        double v1[3], v2[3], v[3];
        double N1[3], N2[3], N[3];
        double alpha;

        // Compute distance signs  of p1, q1 and r1
        // to the plane of triangle(p2,q2,r2)

        SUB(v1, p2, r2)
            SUB(v2, q2, r2)
            CROSS(N2, v1, v2)

            SUB(v1, p1, r2)
            dp1 = DOT(v1, N2);
        SUB(v1, q1, r2)
            dq1 = DOT(v1, N2);
        SUB(v1, r1, r2)
            dr1 = DOT(v1, N2);

        if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f))  return 0;

        // Compute distance signs  of p2, q2 and r2
        // to the plane of triangle(p1,q1,r1)


        SUB(v1, q1, p1)
            SUB(v2, r1, p1)
            CROSS(N1, v1, v2)

            SUB(v1, p2, r1)
            dp2 = DOT(v1, N1);
        SUB(v1, q2, r1)
            dq2 = DOT(v1, N1);
        SUB(v1, r2, r1)
            dr2 = DOT(v1, N1);

        if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) return 0;

        // Permutation in a canonical form of T1's vertices


        if (dp1 > 0.0f) {
            if (dq1 > 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2, i3, i1, i2, j1, j3, j2)
            else if (dr1 > 0.0f) TRI_TRI_INTER_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2, i2, i3, i1, j1, j3, j2)

            else TRI_TRI_INTER_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2, i1, i2, i3, j1, j2, j3)
        }
        else if (dp1 < 0.0f) {
            if (dq1 < 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2, i3, i1, i2, j1, j2, j3)
            else if (dr1 < 0.0f) TRI_TRI_INTER_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2, i2, i3, i1, j1, j2, j3)
            else TRI_TRI_INTER_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2, i1, i2, i3, j1, j3, j2)
        }
        else {
            if (dq1 < 0.0f) {
                if (dr1 >= 0.0f) TRI_TRI_INTER_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2, i2, i3, i1, j1, j3, j2)
                else TRI_TRI_INTER_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2, i1, i2, i3, j1, j2, j3)
            }
            else if (dq1 > 0.0f) {
                if (dr1 > 0.0f) TRI_TRI_INTER_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2, i1, i2, i3, j1, j3, j2)
                else TRI_TRI_INTER_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2, i2, i3, i1, j1, j2, j3)
            }
            else {
                if (dr1 > 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2, i3, i1, i2, j1, j2, j3)
                else if (dr1 < 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2, i3, i1, i2, j1, j3, j2)
                else {
                    // triangles are co-planar

                    *coplanar = 1;
                    return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
                }
            }
        }
    };





    /*
    *
    *  Two dimensional Triangle-Triangle Overlap Test
    *
    */


    /* some 2D macros */

#define ORIENT_2D(a, b, c)  ((a[0]-c[0])*(b[1]-c[1])-(a[1]-c[1])*(b[0]-c[0]))


#define INTERSECTION_TEST_VERTEX(P1, Q1, R1, P2, Q2, R2) {\
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f)\
    if (ORIENT_2D(R2,Q2,Q1) <= 0.0f)\
      if (ORIENT_2D(P1,P2,Q1) > 0.0f) {\
  if (ORIENT_2D(P1,Q2,Q1) <= 0.0f) return 1; \
  else return 0;} else {\
  if (ORIENT_2D(P1,P2,R1) >= 0.0f)\
    if (ORIENT_2D(Q1,R1,P2) >= 0.0f) return 1; \
    else return 0;\
  else return 0;}\
    else \
      if (ORIENT_2D(P1,Q2,Q1) <= 0.0f)\
  if (ORIENT_2D(R2,Q2,R1) <= 0.0f)\
    if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) return 1; \
    else return 0;\
  else return 0;\
      else return 0;\
  else\
    if (ORIENT_2D(R2,P2,R1) >= 0.0f) \
      if (ORIENT_2D(Q1,R1,R2) >= 0.0f)\
  if (ORIENT_2D(P1,P2,R1) >= 0.0f) return 1;\
  else return 0;\
      else \
  if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) {\
    if (ORIENT_2D(R2,R1,Q2) >= 0.0f) return 1; \
    else return 0; }\
  else return 0; \
    else  return 0; \
 };



#define INTERSECTION_TEST_EDGE(P1, Q1, R1, P2, Q2, R2) { \
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f) {\
    if (ORIENT_2D(P1,P2,Q1) >= 0.0f) { \
        if (ORIENT_2D(P1,Q1,R2) >= 0.0f) return 1; \
        else return 0;} else { \
      if (ORIENT_2D(Q1,R1,P2) >= 0.0f){ \
  if (ORIENT_2D(R1,P1,P2) >= 0.0f) return 1; else return 0;} \
      else return 0; } \
  } else {\
    if (ORIENT_2D(R2,P2,R1) >= 0.0f) {\
      if (ORIENT_2D(P1,P2,R1) >= 0.0f) {\
  if (ORIENT_2D(P1,R1,R2) >= 0.0f) return 1;  \
  else {\
    if (ORIENT_2D(Q1,R1,R2) >= 0.0f) return 1; else return 0;}}\
      else  return 0; }\
    else return 0; }}



    int ccw_tri_tri_intersection_2d(double p1[2], double q1[2], double r1[2],
        double p2[2], double q2[2], double r2[2]) {
        if (ORIENT_2D(p2, q2, p1) >= 0.0f) {
            if (ORIENT_2D(q2, r2, p1) >= 0.0f) {
                if (ORIENT_2D(r2, p2, p1) >= 0.0f) return 1;
                else INTERSECTION_TEST_EDGE(p1, q1, r1, p2, q2, r2)
            }
            else {
                if (ORIENT_2D(r2, p2, p1) >= 0.0f)
                    INTERSECTION_TEST_EDGE(p1, q1, r1, r2, p2, q2)
                else INTERSECTION_TEST_VERTEX(p1, q1, r1, p2, q2, r2)
            }
        }
        else {
            if (ORIENT_2D(q2, r2, p1) >= 0.0f) {
                if (ORIENT_2D(r2, p2, p1) >= 0.0f)
                    INTERSECTION_TEST_EDGE(p1, q1, r1, q2, r2, p2)
                else  INTERSECTION_TEST_VERTEX(p1, q1, r1, q2, r2, p2)
            }
            else INTERSECTION_TEST_VERTEX(p1, q1, r1, r2, p2, q2)
        }
    };


    int tri_tri_overlap_test_2d(double p1[2], double q1[2], double r1[2],
        double p2[2], double q2[2], double r2[2]) {
        if (ORIENT_2D(p1, q1, r1) < 0.0f)
            if (ORIENT_2D(p2, q2, r2) < 0.0f)
                return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, r2, q2);
            else
                return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, q2, r2);
        else
            if (ORIENT_2D(p2, q2, r2) < 0.0f)
                return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, r2, q2);
            else
                return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, q2, r2);

    };

    bool isTriangleIntersect(Vec3 t0_p0, Vec3 t0_p1, Vec3 t0_p2,
        Vec3 t1_p0, Vec3 t1_p1, Vec3 t1_p2,
        int i1 , int i2, int i3, int j1, int j2, int j3)
    {
        const float EPSILON = 1e-7f;

        t0_p0 = t0_p0 * 1000.0f;
        t0_p1 = t0_p1 * 1000.0f;
        t0_p2 = t0_p2 * 1000.0f;

        t1_p0 = t1_p0 * 1000.0f;
        t1_p1 = t1_p1 * 1000.0f;
        t1_p2 = t1_p2 * 1000.0f;


        Vec3 N0 = cross(t0_p1 - t0_p0, t0_p2 - t0_p0);
        float lenN0 = length(N0);
        if (lenN0 < EPSILON)
        {
            return false;
        }
        N0 = N0 / lenN0;

        Vec3 N1 = cross(t1_p1 - t1_p0, t1_p2 - t1_p0);
        float lenN1 = length(N1);
        if (lenN1 < EPSILON)
        {
            return false;
        }
        N1 = N1 / lenN1;

        float angle_for_n0_n1 = dot(N1, N0);
        //test if N1 are N0 are on the same line
        if ((angle_for_n0_n1 > 0 ? angle_for_n0_n1 : -angle_for_n0_n1) >= 1 - EPSILON)
            return false;

        //test intersection of triangles
        int coplanar = 0;
        double p0[3] = { t0_p0.x, t0_p0.y, t0_p0.z };
        double p1[3] = { t0_p1.x, t0_p1.y, t0_p1.z };
        double p2[3] = { t0_p2.x, t0_p2.y, t0_p2.z };
        double q0[3] = { t1_p0.x, t1_p0.y, t1_p0.z };
        double q1[3] = { t1_p1.x, t1_p1.y, t1_p1.z };
        double q2[3] = { t1_p2.x, t1_p2.y, t1_p2.z };
        int source_v0;
        int source_v1;

        int target_v0;
        int target_v1;

        bool srcOnT0 = false;
        bool tgtOnT0 = false;
        double source[3];
        double target[3];

        bool intersected = tri_tri_intersection_test_3d(p0, p1, p2, q0, q1, q2, &coplanar, source, target,
            i1,i2,i3,j1,j2,j3,
            source_v0, source_v1, target_v0, target_v1, srcOnT0, tgtOnT0);

        if ( fabs(source[0] - target[0]) < FLT_EPSILON
            && fabs(source[1] - target[1]) < FLT_EPSILON
            && fabs(source[2] - target[2]) < FLT_EPSILON)
        {
            return false;
        }

        if (intersected && !coplanar)
        {
            //printf("untangle tri-hit \n");

            return true;
        }


        return false;
    }


    //--------------------------------------------------------------------------------------------------------------------
}
