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

    class IRenderTexture;
    class CommandBuffer;
    class ITexture2D;
    class IMaterial;
    class IShader;
    class IMesh;

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
        // Set the multisample count from the screens backbuffers
        //----------------------------------------------------------------------
        virtual void setMultiSampleCount(U32 numSamples) = 0;

        //----------------------------------------------------------------------
        virtual void setVSync(bool enabled) = 0;

        //----------------------------------------------------------------------
        virtual IMesh*          createMesh() = 0;
        virtual IMaterial*      createMaterial() = 0;
        virtual IShader*        createShader() = 0;
        virtual ITexture2D*     createTexture2D(U32 width, U32 height, TextureFormat format, bool generateMips) = 0;
        virtual IRenderTexture* createRenderTexture(U32 width, U32 height, U32 depth, TextureFormat format) = 0;

        //----------------------------------------------------------------------
        // Add a global shader to this renderer. A global shader allows to render
        // the whole scene with just one shader-setup. To use it call setGlobalMaterialActive(name)
        // @Params:
        //  "name": The name of this shader to identify it.
        //  "material": The actual material.
        //----------------------------------------------------------------------
        void addGlobalMaterial(CString name, IMaterial* material);

        //----------------------------------------------------------------------
        // Set a global shader with the given name as active. If a global shader
        // is set, every geometry will be rendered with this shader.
        // @Params:
        //  "name": Name of the global shader. If "NONE", it resets to nullptr.
        //----------------------------------------------------------------------
        void setGlobalMaterialActive(CString name = "NONE");

    protected:
        OS::Window* m_window = nullptr;

        //----------------------------------------------------------------------
        HashMap<StringID, IMaterial*> m_globalMaterials;
        IMaterial*                    m_activeGlobalMaterial = nullptr; // If this is not null the scene should be rendered just with this material

        //----------------------------------------------------------------------
        virtual void OnWindowSizeChanged(U16 w, U16 h) = 0;
    };

} // End namespaces