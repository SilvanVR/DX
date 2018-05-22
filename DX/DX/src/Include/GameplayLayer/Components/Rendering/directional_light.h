#pragma once
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018

    Classical directional light.
    The direction is determined based on the attached transform component.
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/Lighting/directional_light.h"

namespace Components {

    //**********************************************************************
    class DirectionalLight : public IRenderComponent
    {
    public:
        //----------------------------------------------------------------------
        DirectionalLight(F32 intensity = 1.0f, Color color = Color::WHITE);

        //----------------------------------------------------------------------
        Color   getColor()      const { return m_dirLight.getColor(); }
        F32     getIntensity()  const { return m_dirLight.getIntensity(); }

        //----------------------------------------------------------------------
        void setIntensity   (F32 intensity) { m_dirLight.setIntensity(intensity); }
        void setColor       (Color color)   { m_dirLight.setColor(color); }

    private:
        Graphics::DirectionalLight m_dirLight;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Camera& camera) override { return true; }

        //----------------------------------------------------------------------
        DirectionalLight(const DirectionalLight& other)               = delete;
        DirectionalLight& operator = (const DirectionalLight& other)  = delete;
        DirectionalLight(DirectionalLight&& other)                    = delete;
        DirectionalLight& operator = (DirectionalLight&& other)       = delete;
    };


} // End namespaces