#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.hpp)

    author: S. Hau
    date: November 28, 2017

    Interface for a renderer subsystem.
**********************************************************************/

#include "Core/i_subsystem.hpp"

//----------------------------------------------------------------------
namespace Core { namespace OS { class Window; } }


namespace Core { namespace Graphics {

    //**********************************************************************
    // Interface-Class for a Renderer-Subsystem
    //**********************************************************************
    class IRenderer : public ISubSystem
    {
    public:
        IRenderer(OS::Window* window) : m_window( window ) {}
        virtual ~IRenderer() {}


        virtual void render() = 0;

    protected:
        OS::Window* m_window;

    };




} } // End namespaces