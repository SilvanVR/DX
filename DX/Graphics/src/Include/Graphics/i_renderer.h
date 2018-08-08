#pragma once
/**********************************************************************
    class: IRenderer (i_renderer.h)

    author: S. Hau
    date: November 28, 2017

    Interface for a renderer subsystem.
**********************************************************************/

#include "forward_declarations.hpp"
#include "Common/i_subsystem.hpp"
#include "OS/Window/window.h"
#include "Events/event.h"
#include "structs.hpp"

namespace Graphics {

    //----------------------------------------------------------------------
    struct Limits
    {
        U32 maxLights     = 16;
        U32 maxShadowmaps = 4;
        U32 maxCascades   = 4;
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
        const OS::Window*       getWindow()         const { return m_window; }
        const U64               getFrameCount()     const { return m_frameCount; }
        const Limits&           getLimits()         const { return m_limits; }
        bool                    isVSyncEnabled()    const { return m_vsync; }
        const GPUDescription&   getGPUDescription() const { return m_gpuDescription; }
        VR::HMD&                getVRDevice()       const { return *m_hmd; }
        bool                    hasHMD()            const { return m_hmd != nullptr; }

        //----------------------------------------------------------------------
        void setVSync(bool enabled) { m_vsync = enabled; }

        //----------------------------------------------------------------------
        // Dispatches the given command buffer for execution on the gpu.
        //----------------------------------------------------------------------
        void dispatch(const CommandBuffer& cmd);

        //----------------------------------------------------------------------
        // Presents the latest backbuffer to the screen.
        //----------------------------------------------------------------------
        virtual void present() = 0;

        //----------------------------------------------------------------------
        // @Return: Which Graphics-API is used by this renderer.
        //----------------------------------------------------------------------
        virtual API getAPI() const = 0;

        //----------------------------------------------------------------------
        // @Return: Used API as an readable string
        //----------------------------------------------------------------------
        virtual String getAPIName() const = 0;

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
        void addGlobalMaterial(CString name, const std::shared_ptr<IMaterial>& material);

        //----------------------------------------------------------------------
        // Set a global shader with the given name as active. If a global shader
        // is set, every geometry will be rendered with this shader.
        // @Params:
        //  "name": Name of the global shader. If "NONE", it resets to nullptr.
        //----------------------------------------------------------------------
        void setGlobalMaterialActive(CString name = "NONE");

    protected:
        U64                         m_frameCount = 0;
        OS::Window*                 m_window;
        ArrayList<CommandBuffer>    m_pendingCmdQueue;
        Limits                      m_limits;
        bool                        m_vsync = false;
        GPUDescription              m_gpuDescription;
        VR::HMD*                    m_hmd = nullptr;
        Events::EventListener       m_windowResizeListener;

        //----------------------------------------------------------------------
        void _LockQueue();
        void _UnlockQueue();

        //----------------------------------------------------------------------
        // Queries all supported HMDs, checks if they are supported and initializes the first supported one.
        // @Return: Whether an hmd was successfully found and initialized.
        //----------------------------------------------------------------------
        bool _InitializeHMD();

        //----------------------------------------------------------------------
        virtual void OnWindowSizeChanged(U16 w, U16 h) = 0;

        //----------------------------------------------------------------------
        HashMap<StringID, std::shared_ptr<IMaterial>>   m_globalMaterials;
        std::shared_ptr<IMaterial>                      m_activeGlobalMaterial = nullptr; // If this is not null the scene should be rendered just with this material

    private:
        void _OnWindowSizeChanged();

        NULL_COPY_AND_ASSIGN(IRenderer)
    };

} // End namespaces