#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.h)

    author: S. Hau
    date: November 28, 2017

    Interface for a renderer subsystem.
**********************************************************************/

#include "Common/i_subsystem.hpp"
#include "i_render_texture.h"
#include "OS/Window/window.h"
#include "structs.hpp"

namespace Graphics {

    //----------------------------------------------------------------------
    class ITexture2DArray;
    class IRenderTexture;
    class IRenderBuffer;
    class CommandBuffer;
    class ITexture2D;
    class IMaterial;
    class ITexture;
    class ICubemap;
    class IShader;
    class IMesh;

    //----------------------------------------------------------------------
    struct FrameInfo
    {
        U32 drawCalls;
        U32 numVertices;
        U32 numTriangles;
        U32 numLights;
    };

    //**********************************************************************
    // Interface-Class for a Renderer-Subsystem
    //**********************************************************************
    class IRenderer : public Core::ISubSystem
    {
    public:
        IRenderer(OS::Window* window);
        virtual ~IRenderer() {}

        //----------------------------------------------------------------------
        const OS::Window*       getWindow()             const { return m_window; }

        //----------------------------------------------------------------------
        // Dispatches the given command buffer for execution on the gpu.
        //----------------------------------------------------------------------
        void dispatch(const CommandBuffer& cmd);

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
        virtual bool isVSyncEnabled() const = 0;


        //----------------------------------------------------------------------
        virtual IMesh*              createMesh() = 0;
        virtual IMaterial*          createMaterial() = 0;
        virtual IShader*            createShader() = 0;
        virtual ITexture2D*         createTexture2D() = 0;
        virtual IRenderTexture*     createRenderTexture() = 0;
        virtual ICubemap*           createCubemap() = 0;
        virtual ITexture2DArray*    createTexture2DArray() = 0;
        virtual IRenderBuffer*      createRenderBuffer() = 0;

        //----------------------------------------------------------------------
        // Update the global buffer.
        // @Return:
        //  False, if the uniform with "name" or a global buffer does not exist.
        //----------------------------------------------------------------------
        virtual bool setGlobalFloat(StringID name, F32 value) = 0;
        virtual bool setGlobalInt(StringID name, I32 value) = 0;
        virtual bool setGlobalVector4(StringID name, const Math::Vec4& vec4) = 0;
        virtual bool setGlobalColor(StringID name, Color color) = 0;
        virtual bool setGlobalMatrix(StringID name, const DirectX::XMMATRIX& matrix) = 0;

        //----------------------------------------------------------------------
        // Add a global shader to this renderer. A global shader allows to render
        // the whole scene with just one shader-setup. To use it call setGlobalMaterialActive(name).
        // Please hold on the reference of the material you are passing in. The renderer does NOT delete it.
        // P.S. This is an ugly hack and will be removed in future versions.
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

        //----------------------------------------------------------------------
        // @Return:
        //  Global information about the last rendered frame.
        //----------------------------------------------------------------------
        const FrameInfo& getLastFrameInfo() const { return m_frameInfo; }
        void resetFrameInfo() { m_frameInfo = {}; }

    protected:
        OS::Window*                 m_window;
        FrameInfo                   m_frameInfo = {};
        ArrayList<CommandBuffer>    m_pendingCmdQueue;

        void _LockQueue();
        void _UnlockQueue();

        //----------------------------------------------------------------------
        virtual void OnWindowSizeChanged(U16 w, U16 h) = 0;

        //----------------------------------------------------------------------
        HashMap<StringID, IMaterial*>   m_globalMaterials;
        IMaterial*                      m_activeGlobalMaterial = nullptr; // If this is not null the scene should be rendered just with this material

    private:
        void _OnWindowSizeChanged();

        //----------------------------------------------------------------------
        IRenderer(const IRenderer& other)               = delete;
        IRenderer& operator = (const IRenderer& other)  = delete;
        IRenderer(IRenderer&& other)                    = delete;
        IRenderer& operator = (IRenderer&& other)       = delete;
    };

} // End namespaces