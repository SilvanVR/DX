#pragma once
/**********************************************************************
    class: IRenderComponent (IRenderComponent.h)

    author: S. Hau
    date: December 19, 2017

    Every component which emits gpu-commands inherits from this class.
**********************************************************************/

#include "../i_component.h"

namespace Components {

    class IRenderComponent : public IComponent
    {

    public:
        IRenderComponent() = default;
//
        //void addedToGameObject(GameObject* go) override;

    private:

        //----------------------------------------------------------------------
        IRenderComponent(const IRenderComponent& other)               = delete;
        IRenderComponent& operator = (const IRenderComponent& other)  = delete;
        IRenderComponent(IRenderComponent&& other)                    = delete;
        IRenderComponent& operator = (IRenderComponent&& other)       = delete;
    };

}