#pragma once
/**********************************************************************
    class: CModelRenderer (CModelRenderer.h)

    author: S. Hau
    date: December 19, 2017

    Used to render models.
**********************************************************************/

#include "../i_component.h"

namespace Components {

    class CModelRenderer : public IComponent
    {

    public:
        CModelRenderer() = default;

        void recordGraphicsCommands(Core::Graphics::CommandBuffer& cmd, F32 lerp) override;


    private:

        //----------------------------------------------------------------------
        CModelRenderer(const CModelRenderer& other)               = delete;
        CModelRenderer& operator = (const CModelRenderer& other)  = delete;
        CModelRenderer(CModelRenderer&& other)                    = delete;
        CModelRenderer& operator = (CModelRenderer&& other)       = delete;
    };

}