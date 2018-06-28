#pragma once
/**********************************************************************
    class: IRenderComponent (i_render_component.hpp)

    author: S. Hau
    date: March 6, 2018

    Interface for all renderer components.
**********************************************************************/

#include "../i_component.h"

namespace Graphics { class Camera; }

namespace Components {

    //**********************************************************************
    class IRenderComponent : public IComponent
    {
    public:
        IRenderComponent() = default;
        virtual ~IRenderComponent() = default;

        //----------------------------------------------------------------------
        virtual void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) {}
        virtual void recordGraphicsCommandsShadows(Graphics::CommandBuffer& cmd, F32 lerp) {}
        virtual bool cull(const Graphics::Camera& camera) { return true; }

        //----------------------------------------------------------------------
        bool castShadows() const { return m_castShadows; }

        //----------------------------------------------------------------------
        void setCastShadows(bool castShadows) { m_castShadows = castShadows; }

    private:
        bool m_castShadows = true;

        NULL_COPY_AND_ASSIGN(IRenderComponent)
    };

}