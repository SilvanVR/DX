#pragma once
/**********************************************************************
    class: ILightComponent

    author: S. Hau
    date: June 27, 2018

    Interface for all light components.
**********************************************************************/

#include "../i_component.h"
#include "Graphics/Lighting/lights.h"

namespace Core { class RenderSystem; }
namespace Graphics { class Camera; }
class IScene;

namespace Components {

    class Camera;

    //**********************************************************************
    class ILightComponent : public IComponent
    {
    public:
        ILightComponent(Graphics::Light* light);
        ILightComponent(Graphics::Light* light, Graphics::ShadowMapQuality quality);
        virtual ~ILightComponent() = default;

        //----------------------------------------------------------------------
        Color                       getColor()              const { return m_light->getColor(); }
        F32                         getIntensity()          const { return m_light->getIntensity(); }
        bool                        shadowsEnabled()        const { return m_light->shadowsEnabled(); }
        const RenderBufferPtr&      getShadowMap()          const { return m_light->getShadowMap(); }
        Graphics::ShadowMapQuality  getShadowMapQuality()   const { return m_shadowMapQuality; }
        const Graphics::Camera&     getNativeCamera()       const { return *m_camera; }

        //----------------------------------------------------------------------
        void setIntensity       (F32 intensity) { m_light->setIntensity(intensity); }
        void setColor           (Color color)   { m_light->setColor(color); }
        void setShadowMapQuality(Graphics::ShadowMapQuality quality);
        void setShadows         (bool enabled);

    protected:
        std::unique_ptr<Graphics::Light>    m_light             = nullptr;
        std::unique_ptr<Graphics::Camera>   m_camera            = nullptr;
        Graphics::ShadowMapQuality          m_shadowMapQuality  = Graphics::ShadowMapQuality::High;

        virtual void renderShadowMap(const IScene& scene, F32 lerp);
        virtual void _CreateShadowMap(Graphics::ShadowMapQuality) = 0;

    private:
        //----------------------------------------------------------------------
        friend class Core::RenderSystem;
        virtual void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) {}
        virtual bool cull(const Graphics::Camera& camera) { return true; }

        NULL_COPY_AND_ASSIGN(ILightComponent)
    };

}