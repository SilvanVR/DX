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
        DirectionalLight(F32 intensity = 1.0f, Color color = Color::WHITE, bool shadowsEnabled = true);

        //----------------------------------------------------------------------
        F32 getShadowRange()        const { return m_dirLight->getShadowRange(); }

        void setShadowRange        (F32 shadowRange)   { m_dirLight->setShadowRange( shadowRange ); }

    private:
        Graphics::DirectionalLight* m_dirLight;

        //----------------------------------------------------------------------
        // ILightComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override { return true; }
        void renderShadowMap(const IScene& scene, F32 lerp) override;
        void _CreateShadowMap(Graphics::ShadowMapQuality) override;

        //----------------------------------------------------------------------
        void _AdaptOrthographicViewFrustum();

        NULL_COPY_AND_ASSIGN(DirectionalLight)
    };


} // End namespaces