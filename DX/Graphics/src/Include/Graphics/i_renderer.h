#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.h)

    author: S. Hau
    date: November 28, 2017

    Interface for a renderer subsystem.
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "OS/Window/window.h"
#include "structs.hpp"

namespace Graphics {

    class CommandBuffer;
    class Mesh;
    class IndexedMesh;

    //**********************************************************************
    // Interface-Class for a Renderer-Subsystem
    //**********************************************************************
    class IRenderer : public Core::ISubSystem
    {
    public:
        IRenderer(OS::Window* window);
        virtual ~IRenderer() {}

        //----------------------------------------------------------------------
        // Dispatches the given command buffer for execution on the gpu.
        //----------------------------------------------------------------------
        virtual void dispatch( const CommandBuffer& cmd ) = 0;

        //----------------------------------------------------------------------
        // Presents the latest backbuffer to the screen.
        //----------------------------------------------------------------------
        virtual void present() = 0;

        //----------------------------------------------------------------------
        virtual void setVSync(bool enabled) = 0;
        virtual void setMultiSampleCount(U32 numSamples) = 0;

        //----------------------------------------------------------------------
        virtual Mesh* createMesh(const void* pVertices, U32 sizeInBytes) = 0;
        virtual Mesh* createIndexedMesh(const void* pVertices, U32 sizeInBytes, const void* pIndices, U32 sizeInBytes2, U32 numIndices) = 0;

    protected:
        OS::Window* m_window = nullptr;

        //----------------------------------------------------------------------
        virtual void OnWindowSizeChanged(U16 w, U16 h) = 0;
    };

} // End namespaces