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

    class Camera;

    //**********************************************************************
    // D3D11 Renderer Subsystem.
    //**********************************************************************
    class D3D11Renderer : public IRenderer
    {
    public:
        D3D11Renderer(OS::Window* window) : IRenderer( window ) {}

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;
        void present() override;
        void setVSync(bool enabled) override { m_vsync = enabled; }
        bool isVSyncEnabled() const { return m_vsync; }

        IMesh*              createMesh() override;
        IMaterial*          createMaterial() override;
        IShader*            createShader() override;
        ITexture2D*         createTexture2D() override;
        IRenderTexture*     createRenderTexture() override;
        ICubemap*           createCubemap() override;
        ITexture2DArray*    createTexture2DArray() override;
        IRenderBuffer*      createRenderBuffer() override;

        bool setGlobalFloat(StringID name, F32 value) override;
        bool setGlobalInt(StringID name, I32 value) override;
        bool setGlobalVector4(StringID name, const Math::Vec4& vec4) override;
        bool setGlobalColor(StringID name, Color color) override;
        bool setGlobalMatrix(StringID name, const DirectX::XMMATRIX& matrix) override;

    private:
        D3D11::Swapchain*   m_pSwapchain    = nullptr;
        bool                m_vsync         = false;
        IMesh*              m_cubeMesh      = nullptr;

        //----------------------------------------------------------------------
        inline void _SetCamera(Camera* camera);
        inline void _DrawMesh(IMesh* mesh, IMaterial* mat, const DirectX::XMMATRIX& model, U32 subMeshIndex);
        inline void _CopyTexture(ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip);
        inline void _RenderCubemap(ICubemap* cubemap, IMaterial* material, U32 dstMip);
        inline void _Blit(RenderTexturePtr src, RenderTexturePtr dst, IMaterial* material);
        inline void _DrawFullScreenQuad(IMaterial* mat, const D3D11_VIEWPORT& viewport);

        //----------------------------------------------------------------------
        void _InitD3D11();
        void _DeinitD3D11();

        void _CreateDeviceAndContext();
        void _CreateSwapchain( U32 numSamples );

        void _ReportLiveObjects();

        void _FlushLightBuffer();

        void _CreateGlobalBuffer();

        void _ExecuteCommandBuffer(const CommandBuffer& cmd);

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