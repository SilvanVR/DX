#pragma once
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.h)

    author: S. Hau
    date: November 28, 2017

    D3D11 Renderer.
**********************************************************************/

#include "../i_renderer.h"
#include "D3D11.hpp"
#include "D3D11Swapchain.h"

namespace Graphics {

    namespace D3D11 { class RenderTexture; }

    //**********************************************************************
    // D3D11 Renderer Subsystem.
    //**********************************************************************
    class D3D11Renderer : public IRenderer
    {
        static const U8 INITIAL_MSAA_SAMPLES = 4;
        static IRenderTexture* s_currentRenderTarget;

    public:
        D3D11Renderer(OS::Window* window) : IRenderer( window ) {}

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;
        void dispatch(const CommandBuffer& cmd) override;
        void present() override;
        void setVSync(bool enabled) override { m_vsync = enabled; }
        void setMultiSampleCount(U32 numSamples) override;

        IMesh*              createMesh() override;
        IMaterial*          createMaterial() override;
        IShader*            createShader() override;
        ITexture2D*         createTexture2D() override;
        IRenderTexture*     createRenderTexture() override;
        ICubemap*           createCubemap() override;
        ITexture2DArray*    createTexture2DArray() override;

        static IRenderTexture* getCurrentRenderTarget() { return s_currentRenderTarget; }

    private:
        D3D11::Swapchain*   m_pSwapchain    = nullptr;
        bool                m_vsync         = false;

        //----------------------------------------------------------------------
        inline void _SetRenderTarget(IRenderTexture* renderTarget);
        inline void _ClearRenderTarget(const Color& clearColor);
        inline void _SetCameraPerspective(const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar);
        inline void _SetCameraOrtho(const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        inline void _SetViewport(const ViewportRect& viewport);
        inline void _DrawMesh(IMesh* mesh, IMaterial* mat, const DirectX::XMMATRIX& model, U32 subMeshIndex);

        //----------------------------------------------------------------------
        void _InitD3D11();
        void _DeinitD3D11();

        void _CreateDeviceAndContext();
        void _CreateSwapchain( U32 numSamples );

        void _ReportLiveObjects();

        void _SetupConstantBuffers(const OS::Path& vertexPath, const OS::Path& fragPath);

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void OnWindowSizeChanged(U16 w, U16 h) override;

        //----------------------------------------------------------------------
        D3D11Renderer(const D3D11Renderer& other)               = delete;
        D3D11Renderer& operator = (const D3D11Renderer& other)  = delete;
        D3D11Renderer(D3D11Renderer&& other)                    = delete;
        D3D11Renderer& operator = (D3D11Renderer&& other)       = delete;
    };




} // End namespaces