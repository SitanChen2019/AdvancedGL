#ifndef POINT_PICK_H
#define POINT_PICK_H
#include "render_type.h"
#include <functional>
#include <cfloat>

class MousePicking
{
public:
    template<class ParticleType>
    Vector<unsigned> pickVertex(
        const float x, const  float y, const  float radius, const  float screenWidht, const  float screenHeight,
        const Matrix4& viewMatrix, const Matrix4& projMatrix,
        const Vector<ParticleType>& particleSeq,
        const std::function< Vec3(const ParticleType&)> getPositionFunc
    )
    {
        Vector<unsigned> retList;

        float min_distance = FLT_MAX;
        int currentIndex = -1;
        for (const auto& particle : particleSeq)
        {
            ++currentIndex;
            Vec3 pos = getPositionFunc(particle);
            Vec3 screenPos = worldPos2ScreenPos(pos, viewMatrix, projMatrix, screenWidht, screenHeight);

            float distance = glm::distance(Vec2(x, y), Vec2(screenPos.x, screenPos.y)) - radius;
            if ( distance < 0 )
            {
                retList.push_back(currentIndex);

                //check if current is nearest
                if (min_distance > distance)
                {
                    min_distance = distance;
                    std::swap(retList.front(), retList.back());
                }
            }
        }

        return retList;
    }







    template<class ParticleType, class TriangleIndice>
    Vector<unsigned> pickTriangle(
        const float x, const  float y, const  float radius, const  float screenWidth, const  float screenHeight,
        const Matrix4& viewMatrix, const Matrix4& projMatrix,
        const Vector<ParticleType>& particleSeq,
        const std::function< Vec3(const Vector<ParticleType>&, size_t)> getPositionFunc,
        const Vector<TriangleIndice>& triangle_indices, const size_t triangle_count,
        const std::function< std::tuple<int, int, int>( const Vector<TriangleIndice>&, size_t)> getTriangleIndicesFunction
    )
    {
        Vector<unsigned> retList;

        float min_zvalue = FLT_MAX;
        for(size_t i = 0; i < triangle_count; ++i )
        {
            std::tuple<unsigned, unsigned, unsigned> triVertexIndices = getTriangleIndicesFunction(triangle_indices, i);

            Vec3 pos0 = getPositionFunc(particleSeq, std::get<0>(triVertexIndices));
            Vec3 pos1 = getPositionFunc(particleSeq, std::get<1>(triVertexIndices));
            Vec3 pos2 = getPositionFunc(particleSeq, std::get<2>(triVertexIndices));

            Vec3 screenPos0 = worldPos2ScreenPos(pos0, viewMatrix, projMatrix, screenWidth, screenHeight);
            Vec3 screenPos1 = worldPos2ScreenPos(pos1, viewMatrix, projMatrix, screenWidth, screenHeight);
            Vec3 screenPos2 = worldPos2ScreenPos(pos2, viewMatrix, projMatrix, screenWidth, screenHeight);

            auto result = testTriangleInPickArea(Vec2(screenPos0.x, screenPos0.y),
                                                 Vec2(screenPos1.x, screenPos1.y), 
                                                 Vec2(screenPos2.x, screenPos2.y),
                                                 Vec2(x,y), radius);
            if (result.first)
            {
                Vec3 uvw = result.second;
                float z_value = glm::dot( uvw, Vec3(screenPos0.z, screenPos1.z, screenPos2.z) );

                if (z_value >= -1 && z_value <= 1 )
                {
                    retList.push_back((int)i);
                    if (z_value < min_zvalue)
                    {
                        min_zvalue = z_value;
                        std::swap(retList.front(), retList.back());
                    }
                }
            }
        }

        return retList;
    }

private:

    Vec3 worldPos2ScreenPos(const Vec3 worldPos, const Matrix4& viewMat, const Matrix4& projMat, const float screenWidth, const float screenHeight)
    {
        glm::vec4 tmp = projMat * viewMat * Vec4(worldPos, 1);
        tmp /= tmp.w;

        tmp.x = (tmp.x + 1) * 0.5f;
        tmp.y = (tmp.y + 1) * 0.5f;
        tmp.y = 1 - tmp.y;

        return Vec3(screenWidth * tmp.x, screenHeight * tmp.y, tmp.z);
    }

    std::pair<bool, Vec3> testTriangleInPickArea(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2 mousePt, float radius)
    {
        Vec2 v0 = p1 - p0;
        Vec2 v1 = p2 - p0;  
        Vec2 v2 = mousePt - p0;

        float v0v0 = glm::dot(v0, v0);
        float v1v0 = glm::dot(v0, v1);
        float v1v1 = glm::dot(v1, v1);
        float v2v0 = glm::dot(v2, v0);
        float v2v1 = glm::dot(v2, v1);

        float denom = v0v0 * v1v1 - v1v0 * v1v0;

        float v = (v2v0 * v1v1 - v2v1 * v1v0) / denom;
        float w = -(v2v0 * v1v0 - v2v1 * v0v0) / denom;

        float u = 1 - v - w;

        Vec3 uvw = Vec3(u, v, w);
        if (v >= 0 && v <= 1 && w >= 0 && w <= 1 && u >= 0 && u <= 1)
        {
            return std::make_pair(true, uvw);
        }
        else
        {
            return std::make_pair(false, uvw);
        }
    }
};
#endif