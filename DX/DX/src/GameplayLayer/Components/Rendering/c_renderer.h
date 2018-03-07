#pragma once
/**********************************************************************
    class: CRenderer (model_renderer.h)

    author: S. Hau
    date: March 6, 2018

    Interface for all renderer components.
**********************************************************************/

#include "../i_component.h"

namespace Components {

    //**********************************************************************
    class CRenderer : public IComponent
    {
        static ArrayList<CRenderer*> s_renderer;

    public:
        CRenderer();
        virtual ~CRenderer();

        //----------------------------------------------------------------------
        static const ArrayList<CRenderer*>& GetAll() { return s_renderer; }

    private:
        //----------------------------------------------------------------------
        CRenderer(const CRenderer& other)               = delete;
        CRenderer& operator = (const CRenderer& other)  = delete;
        CRenderer(CRenderer&& other)                    = delete;
        CRenderer& operator = (CRenderer&& other)       = delete;
    };

}