#pragma once
/**********************************************************************
    class: ILightComponent (i_light_component.hpp)

    author: S. Hau
    date: June 27, 2018

    Interface for all light components.
**********************************************************************/

#include "../i_component.h"
#include "Graphics/Lighting/light.h"
#include "Graphics/camera.h"

class IScene;

namespace Components {

    class Camera;

    //**********************************************************************
    class ILightComponent : public IComponent
    {
    public:
        ILightComponent(Graphics::Light* light) : m_light{ light } {}
        virtual ~ILightComponent() = default;

        //----------------------------------------------------------------------
        Color                   getColor()          const { return m_light->getColor(); }
        F32                     getIntensity()      const { return m_light->getIntensity(); }
        bool                    shadowsEnabled()    const { return m_light->shadowsEnabled(); }
        const RenderBufferPtr&  getShadowMap()      const { return m_light->getShadowMap(); }

        void setIntensity   (F32 intensity) { m_light->setIntensity(intensity); }
        void setColor       (Color color)   { m_light->setColor(color); }


    protected:
        std::unique_ptr<Graphics::Light> m_light;
        Graphics::Camera m_camera;

    private:
        //----------------------------------------------------------------------
        friend class Camera;
        virtual void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) {}
        virtual bool cull(const Graphics::Camera& camera) { return true; }
        virtual void renderShadowMap(const IScene& scene, F32 lerp){}

        NULL_COPY_AND_ASSIGN(ILightComponent)
    };

}