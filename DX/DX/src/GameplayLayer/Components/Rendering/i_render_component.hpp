#pragma once
/**********************************************************************
    class: IRenderComponent (model_renderer.h)

    author: S. Hau
    date: March 6, 2018

    Interface for all renderer components.
**********************************************************************/

#include "../i_component.h"

namespace Components {

    class Camera;

    //**********************************************************************
    class IRenderComponent : public IComponent
    {
    public:
        IRenderComponent() = default;
        virtual ~IRenderComponent() = default;

        //----------------------------------------------------------------------
        virtual void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) {}
        virtual bool cull(const Camera& camera) { return true; }

    private:
        //----------------------------------------------------------------------
        IRenderComponent(const IRenderComponent& other)               = delete;
        IRenderComponent& operator = (const IRenderComponent& other)  = delete;
        IRenderComponent(IRenderComponent&& other)                    = delete;
        IRenderComponent& operator = (IRenderComponent&& other)       = delete;
    };

}