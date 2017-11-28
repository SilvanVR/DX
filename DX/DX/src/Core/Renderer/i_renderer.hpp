#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.hpp)

    author: S. Hau
    date: November 28, 2017

    Interface for a renderer subsystem.
**********************************************************************/

#include "Core/i_subsystem.hpp"

namespace Core { namespace Graphics {


    //**********************************************************************
    // Interface-Class for a Renderer-Subsystem
    //**********************************************************************
    class IRenderer : public ISubSystem
    {
    public:
        virtual ~IRenderer() {}


        virtual void render() = 0;

    private:

    };




} } // End namespaces