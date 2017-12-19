#pragma once
/**********************************************************************
    class: CModelRenderer (CModelRenderer.h)

    author: S. Hau
    date: December 19, 2017

    Every component which emits gpu-commands inherits from this class.
**********************************************************************/

#include "i_render_component.hpp"

namespace Components {

    class CModelRenderer : public IRenderComponent
    {

    public:
        CModelRenderer() = default;

    private:

        //----------------------------------------------------------------------
        CModelRenderer(const CModelRenderer& other)               = delete;
        CModelRenderer& operator = (const CModelRenderer& other)  = delete;
        CModelRenderer(CModelRenderer&& other)                    = delete;
        CModelRenderer& operator = (CModelRenderer&& other)       = delete;
    };

}