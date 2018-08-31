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
#include "Pipeline/Buffers/D3D11MappedConstantBuffer.h"

namespace Graphics {

    //----------------------------------------------------------------------
    #define MAX_LIGHTS              16
    #define MAX_SHADOWMAPS_2D       4
    #define MAX_SHADOWMAPS_3D       1
    #define MAX_SHADOWMAPS_ARRAY    1
    #define MAX_CSM_SPLITS          4

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

        API getAPI() const override { return API::D3D11; }
        String getAPIName() const override { return "Direct3D11"; }

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

        D3D11::MappedConstantBuffer* m_objectBuffer = nullptr;
        D3D11::MappedConstantBuffer* m_globalBuffer = nullptr;
        D3D11::MappedConstantBuffer* m_cameraBuffer = nullptr;
        D3D11::MappedConstantBuffer* m_lightBuffer  = nullptr;

        //----------------------------------------------------------------------
        inline void _SetCamera(Camera* camera);
        inline void _BindMesh(IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex);
        inline void _DrawMesh(IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& model, I32 subMeshIndex);
        inline void _DrawMeshInstanced(IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& model, I32 instanceCount);
        inline void _CopyTexture(ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip);
        inline void _RenderCubemap(ICubemap* cubemap, const MaterialPtr& material, U32 dstMip);
        inline void _Blit(const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material);
        inline void _DrawFullScreenQuad(const MaterialPtr& material, const D3D11_VIEWPORT& viewport);

        //----------------------------------------------------------------------
        void _InitD3D11();
        void _DeinitD3D11();

        void _CreateDeviceAndContext();
        void _CreateSwapchain();
        void _SetGPUDescription();
        void _CreateRequiredUniformBuffersFromFile(const String& engineVS, const String& engineFS);
        void _ReportLiveObjects();
        void _CreateCubeMesh();
        void _SetLimits();

        void _FlushLightBuffer();
        void _ExecuteCommandBuffer(const CommandBuffer& cmd);
        bool _UpdateGlobalBuffer(StringID name, const void* data);

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void OnWindowSizeChanged(U16 w, U16 h) override;

        //----------------------------------------------------------------------
        struct RenderContext
        {
            I32          lightCount = 0;
            const Light* lights[MAX_LIGHTS];
            bool         lightsUpdated = false; // Set to true whenever a new light has been added

            inline void Reset();
            inline void BindMaterial(const MaterialPtr& material);
            inline void BindShader(const std::shared_ptr<IShader>& shader);
            inline void BindRendertarget(const RenderTexturePtr& rt, U64 frameCount);
            inline void SetCamera(Camera* camera);

            inline IShader*         getShader()         const { return m_shader.get(); }
            inline IRenderTexture*  getRenderTarget()   const { return m_renderTarget.get(); }
            inline Camera*          getCamera()         const { return m_camera; }

        private:
            Camera*                     m_camera = nullptr;       // Current camera
            MaterialPtr                 m_material = nullptr;     // Current bound material
            std::shared_ptr<IShader>    m_shader = nullptr;       // Current bound shader
            RenderTexturePtr            m_renderTarget = nullptr; // Current render target
        } renderContext;

        NULL_COPY_AND_ASSIGN(D3D11Renderer)
    };




} // End namespaces