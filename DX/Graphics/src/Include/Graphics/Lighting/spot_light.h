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
        SpotLight(F32 intensity, Color color, const Math::Vec3& position = { 0, 0, 0 }, F32 angle = 90.0f)
            : PointLight( LightType::Spot, intensity, color, position ), m_angle( Math::Deg2Rad( angle ) ) {}
        ~SpotLight() = default;

        //----------------------------------------------------------------------
        inline F32  getAngle() const { return Math::Rad2Deg( m_angle ); }

        //----------------------------------------------------------------------
        inline void setAngle(F32 angleInDegree) { m_angle = Math::Deg2Rad( angleInDegree ); }

    private:
        F32 m_angle;

        //----------------------------------------------------------------------
        SpotLight(const SpotLight& other)                 = delete;
        SpotLight& operator = (const SpotLight& other)    = delete;
        SpotLight(SpotLight&& other)                      = delete;
        SpotLight& operator = (SpotLight&& other)         = delete;
    };

} // End namespaces