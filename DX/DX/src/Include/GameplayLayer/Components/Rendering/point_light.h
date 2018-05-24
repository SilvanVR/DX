#pragma once
/**********************************************************************
    class: PointLight (point_light.h)
    
    author: S. Hau
    date: May 22, 2018

    Classical point light.
    The position is determined based on the attached transform component.
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/Lighting/point_light.h"

namespace Components {

    //**********************************************************************
    class PointLight : public IRenderComponent
    {
    public:
        //----------------------------------------------------------------------
        PointLight(F32 intensity = 1.0f, Color color = Color::WHITE);

        //----------------------------------------------------------------------
        F32   getIntensity() const { return m_pointLight.getIntensity(); }
        Color getColor()     const { return m_pointLight.getColor(); }
        F32   getRange()     const { return m_pointLight.getRange(); }

        //----------------------------------------------------------------------
        void setIntensity   (F32 intensity) { m_pointLight.setIntensity(intensity); }
        void setColor       (Color color)   { m_pointLight.setColor(color); }
        void setRange       (F32 range)     { m_pointLight.setRange(range); }

    private:
        Graphics::PointLight m_pointLight;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Camera& camera) override;

        //----------------------------------------------------------------------
        PointLight(const PointLight& other)               = delete;
        PointLight& operator = (const PointLight& other)  = delete;
        PointLight(PointLight&& other)                    = delete;
        PointLight& operator = (PointLight&& other)       = delete;
    };


} // End namespaces