#pragma once
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018
**********************************************************************/

#include "i_render_component.hpp"

namespace Components {

    //**********************************************************************
    class DirectionalLight : public IRenderComponent
    {
    public:
        DirectionalLight(const Math::Vec3& direction, F32 intensity = 1.0f, Color color = Color::WHITE);

    private:
        Math::Vec3  m_direction;
        Color       m_color;
        F32         m_intensity;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Camera& camera) override;

        //----------------------------------------------------------------------
        DirectionalLight(const DirectionalLight& other)               = delete;
        DirectionalLight& operator = (const DirectionalLight& other)  = delete;
        DirectionalLight(DirectionalLight&& other)                    = delete;
        DirectionalLight& operator = (DirectionalLight&& other)       = delete;
    };


} // End namespaces