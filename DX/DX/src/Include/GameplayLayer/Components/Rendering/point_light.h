#pragma once
/**********************************************************************
    class: PointLight (point_light.h)
    
    author: S. Hau
    date: May 22, 2018

    Classical point light.
    The position is determined based on the attached transform component.
**********************************************************************/

#include "i_light_component.hpp"
#include "Graphics/Lighting/point_light.h"

namespace Components {

    //**********************************************************************
    class PointLight : public ILightComponent
    {
    public:
        //----------------------------------------------------------------------
        PointLight(F32 intensity = 1.0f, Color color = Color::WHITE, F32 range = 10.0f);

        //----------------------------------------------------------------------
        F32 getRange() const { return m_pointLight->getRange(); }

        //----------------------------------------------------------------------
        void setRange(F32 range) { m_pointLight->setRange(range); }

    private:
        Graphics::PointLight* m_pointLight;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(PointLight)
    };


} // End namespaces