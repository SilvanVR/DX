#pragma once
/**********************************************************************
    class: PointLight (point_light.h)

    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "light.h"

namespace Graphics
{

    //**********************************************************************
    class PointLight : public Light
    {
    public:
        PointLight(F32 intensity, Color color, const Math::Vec3& position = { 0, 0, 0 })
            : Light( LightType::Point, intensity, color ), m_position( position ) {}
        ~PointLight() = default;

        //----------------------------------------------------------------------
        inline const Math::Vec3&        getPosition()               const { return m_position; }
        inline const LightAttenuation&  getAttenuation()            const { return m_attenuation; }
        inline F32                      getConstantAttenuation()    const { return m_attenuation.constant; }
        inline F32                      getLinearAttenuation()      const { return m_attenuation.linear; }
        inline F32                      getQuadraticAttenuation()   const { return m_attenuation.quadratic; }

        //----------------------------------------------------------------------
        inline void setPosition(const Math::Vec3& pos)                  { m_position = pos; }
        inline void setConstantAttenuation(F32 constant)                { m_attenuation.constant = constant; }
        inline void setLinearAttenuation(F32 linear)                    { m_attenuation.linear = linear; }
        inline void setQuadraticAttenuation(F32 quadratic)              { m_attenuation.quadratic = quadratic; }
        inline void setAttenuation(F32 constant, F32 linear, F32 quad)  { m_attenuation = { constant, linear, quad }; }

    protected:
        // Constructor for spot-light
        PointLight(LightType lightType, F32 intensity, Color color, const Math::Vec3& position)
            : Light( lightType, intensity, color ), m_position( position ) {}

    private:
        Math::Vec3       m_position     = { 0, 0, 0 };
        LightAttenuation m_attenuation  = { 0, 1, 0 };

        //----------------------------------------------------------------------
        PointLight(const PointLight& other)                 = delete;
        PointLight& operator = (const PointLight& other)    = delete;
        PointLight(PointLight&& other)                      = delete;
        PointLight& operator = (PointLight&& other)         = delete;
    };

} // End namespaces