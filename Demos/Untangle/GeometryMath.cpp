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
		/*
		* In the second step we try to test if intersectPt is inside the triangle.
		* for any point p on the triangle plane,we can get three vector( v0-p, v1-p , v2-p), we say it (u, v ,k)
		* If we choose any two vector (if they are not on the same line) as axis of a 2-D coordindte system,
		* there must be some a and b make it happen : k = a*u + b*v.
		* the third vector must be in the third quadrant if p is inside the triangle.
		* So we can test if a < 0 and b < 0
		*/

		auto pV0 = m_v0 - intersectPt;
		auto pV1 = m_v1 - intersectPt;
		auto pV2 = m_v2 - intersectPt;

		auto u = pV0;
		auto v = pV1;
		auto k = pV2;

		/*
		Ux*a + Vx*b = Kx  ------- formual_1
		Uy*a + Vy*b = Ky  ------- formual_2
		Uz*a + Vz*b = Kz  ------- formual_3
		*/
		REAL denominator, numerator_a, numerator_b;


		REAL denominator_12 = fabs(u.x * v.y - u.y * v.x);
		REAL denominator_13 = fabs(u.x * v.z - u.z * v.x);
		REAL denominator_23 = fabs(u.y * v.z - u.z * v.y);
		if (denominator_12 > denominator_13&& denominator_12 >= denominator_23)
		{
			denominator = u.x * v.y - u.y * v.x;
			numerator_a = k.x * v.y - k.y * v.x;
			numerator_b = -k.x * u.y + k.y * u.x;
		}
		else if (denominator_13 > denominator_12&& denominator_13 >= denominator_23)
		{
			denominator = u.x * v.z - u.z * v.x;
			numerator_a = k.x * v.z - k.z * v.x;
			numerator_b = -k.x * u.z + k.z * u.x;
		}
		else
		{
			denominator = u.y * v.z - u.z * v.y;
			numerator_a = k.y * v.z - k.z * v.y;
			numerator_b = -k.y * u.z + k.z * u.y;
		}

		if (isDifferentSignal(numerator_a, denominator) && isDifferentSignal(numerator_b, denominator))
			return true;
		else
			return false;
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
 
        if (glm::dot(e0t0, n) > 0
            && glm::dot(e1t0, n) < 0)
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
				return testT0T1crossE0E1(r0, r1) || testT0T1crossE0E1(r0, r2) || testT0T1crossE0E1(r1, r2);
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
					if ( (sign_key != 1) && (sign_key != 2) )
						return false;
					
					float beta12 = D2 / (D2 - D1);
					t0 = beta12 * r1 + (1 - beta12) * r2;
					t1 = r0;

				}
				else if (near_zero_num == 2) //D1 is zero
				{
					if ((sign_key != 4) && (sign_key != 1))
						return false;
	
					float beta02 = D2 / (D2 - D0);
					t0 = beta02 * r0 + (1 - beta02) * r2;
					t1 = r1;
				}
				else if (near_zero_num == 1) //D2 is zero
				{
					if ((sign_key != 4) && (sign_key != 2))
						return false;

					float beta01 = D1 / (D1 - D0);
					t0 = beta01 * r0 + (1 - beta01) * r1;
					t1 = r2;
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
        Triangle t0(p00, p01, p02);
        Triangle t1(p10, p11, p12);
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


}

