#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.h)

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
        IRenderer(OS::Window* window);
        virtual ~IRenderer() {}

        //----------------------------------------------------------------------
        virtual void render() = 0;

        //----------------------------------------------------------------------
        virtual void setVSync(bool enabled) = 0;
        virtual void setMultiSampleCount(U32 numSamples) = 0;

    protected:
        OS::Window* m_window    = nullptr;

        //----------------------------------------------------------------------
        virtual void OnWindowSizeChanged(U16 w, U16 h) = 0;
    };




} } // End namespaces