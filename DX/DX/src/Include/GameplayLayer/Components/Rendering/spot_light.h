#pragma once
/**********************************************************************
    class: SpotLight (spot_light.h)
    
    author: S. Hau
    date: May 22, 2018

    Classical spot light.
    The position/rotation is determined based on the attached transform component.
**********************************************************************/

#include "i_light_component.h"
#include "Graphics/Lighting/lights.h"

namespace Components {

    //**********************************************************************
    class SpotLight : public ILightComponent
    {
    public:
        //----------------------------------------------------------------------
        SpotLight(F32 intensity = 1.0f, Color color = Color::WHITE, F32 spotAngleInDegrees = 45.0f, F32 range = 10.0f, bool shadowsEnabled = true);

        //----------------------------------------------------------------------
        F32     getRange()      const { return m_spotLight->getRange(); }
        F32     getAngle()      const;

        //----------------------------------------------------------------------
        void setRange       (F32 range)     { m_spotLight->setRange(range); }
        void setAngle       (F32 angle);

    private:
        Graphics::SpotLight* m_spotLight;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;
        void _CreateShadowMap(Graphics::ShadowMapQuality) override;

        NULL_COPY_AND_ASSIGN(SpotLight)
    };


} // End namespaces