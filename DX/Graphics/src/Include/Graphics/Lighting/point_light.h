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
        PointLight(F32 intensity, Color color, const Math::Vec3& position = { 0, 0, 0 }, F32 range = 5.0f )
            : Light( LightType::Point, intensity, color ), m_position( position ), m_range( range ) {}
        ~PointLight() = default;

        //----------------------------------------------------------------------
        inline const Math::Vec3& getPosition()   const { return m_position; }
        inline F32               getRange()      const { return m_range; }

        //----------------------------------------------------------------------
        inline void setPosition (const Math::Vec3& pos) { m_position = pos; }
        inline void setRange    (F32 range)             { m_range = range; }

    protected:
        // Constructor for spot-light
        PointLight(LightType lightType, F32 intensity, Color color, const Math::Vec3& position, F32 range)
            : Light( lightType, intensity, color ), m_position( position ), m_range( range ) {}

    private:
        Math::Vec3   m_position = { 0, 0, 0 };
        F32          m_range    = 5.0f;

        NULL_COPY_AND_ASSIGN(PointLight)
    };

} // End namespaces