#pragma once
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018

    Classical directional light.
    The direction is determined based on the attached transform component.
**********************************************************************/

#include "i_light_component.h"
#include "Graphics/Lighting/lights.h"

namespace Components {

    //**********************************************************************
    class DirectionalLight : public ILightComponent
    {
    public:
        //----------------------------------------------------------------------
        DirectionalLight(F32 intensity = 1.0f, Color color = Color::WHITE);
        DirectionalLight(F32 intensity, Color color, Graphics::ShadowMapQuality quality);

    private:
        Graphics::DirectionalLight* m_dirLight;

        //----------------------------------------------------------------------
        // ILightComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override { return true; }
        void renderShadowMap(const IScene& scene, F32 lerp) override;
        void _CreateShadowMap(Graphics::ShadowMapQuality) override;

        NULL_COPY_AND_ASSIGN(DirectionalLight)
    };


} // End namespaces