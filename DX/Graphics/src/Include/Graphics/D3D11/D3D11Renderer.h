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

    //----------------------------------------------------------------------
    #define MAX_LIGHTS          16
    #define MAX_SHADOWMAPS_2D   4
    #define MAX_SHADOWMAPS_3D   1

    //----------------------------------------------------------------------
    class Camera;
    class Light;

    //**********************************************************************
    // D3D11 Renderer
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
        IMesh*              m_cubeMesh      = nullptr;

        //----------------------------------------------------------------------
        inline void _SetCamera(Camera* camera);
        inline void _DrawMesh(IMesh* mesh, const std::shared_ptr<IMaterial>& material, const DirectX::XMMATRIX& model, U32 subMeshIndex);
        inline void _CopyTexture(ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip);
        inline void _RenderCubemap(ICubemap* cubemap, const std::shared_ptr<IMaterial>& material, U32 dstMip);
        inline void _Blit(RenderTexturePtr src, RenderTexturePtr dst, const std::shared_ptr<IMaterial>& material);
        inline void _DrawFullScreenQuad(const std::shared_ptr<IMaterial>& material, const D3D11_VIEWPORT& viewport);

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
        struct RenderContext
        {
            Camera*             camera = nullptr;  // Current camera
            RenderTexturePtr    renderTarget = nullptr; // Current render target

            I32          lightCount = 0;
            const Light* lights[MAX_LIGHTS];
            bool         lightsUpdated = false; // Set to true whenever a new light has been added

            void Reset();
            void BindMaterial(const std::shared_ptr<IMaterial>& material);
            void BindShader(const std::shared_ptr<IShader>& shader);

            inline IShader* getShader() const { return m_shader.get(); }

        private:
            std::shared_ptr<IMaterial>  m_material = nullptr;     // Current bound material
            std::shared_ptr<IShader>    m_shader = nullptr;       // Current bound shader
        } renderContext;

        NULL_COPY_AND_ASSIGN(D3D11Renderer)
    };




} // End namespaces