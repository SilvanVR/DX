#pragma once
/**********************************************************************
    class: SpotLight (spot_light.h)

    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "point_light.h"
#include "Math/math_utils.h"

namespace Graphics
{

    //**********************************************************************
    class SpotLight : public PointLight
    {
    public:
        SpotLight(F32 intensity, Color color, const Math::Vec3& position = { 0, 0, 0 }, F32 angleInDeg = 90.0f, const Math::Vec3& direction = { 0, 0, 1 }, F32 range = 10.0f)
            : PointLight( LightType::Spot, intensity, color, position, range ), m_angle( Math::Deg2Rad( angleInDeg ) ), m_direction( direction ) {}
        ~SpotLight() = default;

        //----------------------------------------------------------------------
        inline F32                  getAngle()          const { return m_angle; }
        inline const Math::Vec3&    getDirection()      const { return m_direction; }

        //----------------------------------------------------------------------
        inline void setAngle    (F32 angleInDegree)             { m_angle = Math::Deg2Rad( angleInDegree ); }
        inline void setDirection(const Math::Vec3& direction)   { m_direction = direction; }

    private:
        Math::Vec3  m_direction;
        F32         m_angle;

        NULL_COPY_AND_ASSIGN(SpotLight)
    };

} // End namespaces