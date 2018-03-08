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

    class RenderTexture;

    //**********************************************************************
    // D3D11 Renderer Subsystem.
    //**********************************************************************
    class D3D11Renderer : public IRenderer
    {
        static const U8 INITIAL_MSAA_SAMPLES = 4;

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

        Mesh* createMesh(const void* pVertices, U32 sizeInBytes) override;
        Mesh* createIndexedMesh(const void* pVertices, U32 sizeInBytes, const void* pIndices, U32 sizeInBytes2, U32 numIndices) override;

    private:
        D3D11::Swapchain*   m_pSwapchain    = nullptr;
        bool                m_vsync         = false;

        //----------------------------------------------------------------------
        inline void _SetRenderTarget(RenderTexture* renderTarget);
        inline void _ClearRenderTarget(const Color& clearColor);
        inline void _SetCameraPerspective(const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar);
        inline void _SetCameraOrtho(const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        inline void _SetViewport(const ViewportRect& viewport);
        inline void _DrawMesh(const DirectX::XMMATRIX& model, Mesh* mesh);
        inline void _DrawIndexedMesh(const DirectX::XMMATRIX& model, IndexedMesh* mesh);

        //----------------------------------------------------------------------
        void _InitD3D11();
        void _DeinitD3D11();

        void _CreateDeviceAndContext();
        void _CreateSwapchain( U32 numSamples );

        void _ReportLiveObjects();

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