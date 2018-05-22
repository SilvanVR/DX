#pragma once
/**********************************************************************
    class: DirectionalLight (directional_light.h)

    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "light.h"

namespace Graphics
{

    //**********************************************************************
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(F32 intensity, Color color, const Math::Vec3& direction = { 0, 0, 0 })
            : Light(LightType::Directional, intensity, color), m_direction(direction) {}
        ~DirectionalLight() = default;

        //----------------------------------------------------------------------
        inline const Math::Vec3& getDirection() const { return m_direction; }

        //----------------------------------------------------------------------
        inline void setDirection(const Math::Vec3& dir) { m_direction = dir; }

    private:
        Math::Vec3 m_direction;

        //----------------------------------------------------------------------
        DirectionalLight(const DirectionalLight& other)                 = delete;
        DirectionalLight& operator = (const DirectionalLight& other)    = delete;
        DirectionalLight(DirectionalLight&& other)                      = delete;
        DirectionalLight& operator = (DirectionalLight&& other)         = delete;
    };

} // End namespaces