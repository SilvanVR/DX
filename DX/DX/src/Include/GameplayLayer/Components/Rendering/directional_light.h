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
        DirectionalLight(F32 intensity = 1.0f, Color color = Color::WHITE, Graphics::ShadowType shadowType = Graphics::ShadowType::Soft);
        DirectionalLight(F32 intensity, Color color, Graphics::ShadowType shadowType, const ArrayList<F32>& splitRangesWorldSpace);

        //----------------------------------------------------------------------
        Math::Vec3  getDirection()   const { return m_dirLight->getDirection(); }
        F32         getShadowRange() const { return m_dirLight->getShadowRange(); }

        void setDirection       (const Math::Vec3& dir)         { m_dirLight->setDirection( dir ); }
        void setShadowRange     (F32 shadowRange)               { m_dirLight->setShadowRange(shadowRange); }
        void setCSMSplitRanges  (const ArrayList<F32>& ranges)  { m_dirLight->setCSMSplitRanges(ranges); }

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
        void _AdaptOrthographicViewFrustum(Components::Camera* camera, F32 zNear, F32 zFar);

        NULL_COPY_AND_ASSIGN(DirectionalLight)
    };


} // End namespaces