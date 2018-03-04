#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.h)

    author: S. Hau
    date: November 28, 2017

    Interface for a renderer subsystem.
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "OS/Window/window.h"
#include "Common/color.h"

namespace Graphics {

    class CommandBuffer;

    //**********************************************************************
    // Interface-Class for a Renderer-Subsystem
    //**********************************************************************
    class IRenderer : public Core::ISubSystem
    {
    public:
        IRenderer(OS::Window* window);
        virtual ~IRenderer() {}

        //----------------------------------------------------------------------
        virtual void dispatch( const CommandBuffer& cmd ) = 0;

        //----------------------------------------------------------------------
        virtual void setVSync(bool enabled) = 0;
        virtual void setMultiSampleCount(U32 numSamples) = 0;
        virtual void setClearColor(Color clearColor) = 0;

    protected:
        OS::Window* m_window    = nullptr;

        //----------------------------------------------------------------------
        virtual void OnWindowSizeChanged(U16 w, U16 h) = 0;
    };

} // End namespaces