#pragma once
/**********************************************************************
    class: VkRenderer

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#include "Vulkan.hpp"
#include "../i_renderer.h"
#include "VkSwapchain.h"
#include "Pipeline/VkMappedUniformBuffer.h"

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
    // Vulkan Renderer
    //**********************************************************************
    class VkRenderer : public IRenderer
    {
    public:
        VkRenderer(OS::Window* window) : IRenderer( window ) {}

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;
        void present() override;
        void dispatchImmediate(const CommandBuffer& cmd) override;

        API getAPI() const override { return API::Vulkan; }
        String getAPIName() const override { return "Vulkan"; }

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
        Vulkan::Swapchain   m_swapchain;
        IMesh*              m_cubeMesh      = nullptr;

        Vulkan::MappedUniformBuffer*        m_globalBuffer = nullptr;
        Vulkan::CachedMappedUniformBuffer*  m_cameraBuffer = nullptr;
        Vulkan::CachedMappedUniformBuffer*  m_lightBuffer  = nullptr;

        ITexture2D*         m_fakeShadowMaps2D[MAX_SHADOWMAPS_2D];
        ICubemap*           m_fakeShadowMaps3D[MAX_SHADOWMAPS_3D];
        ITexture2DArray*    m_fakeShadowMaps2DArray[MAX_SHADOWMAPS_ARRAY];

        //----------------------------------------------------------------------
        inline void _SetCamera(Camera* camera);
        inline void _BindMesh(IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex);
        inline void _DrawMesh(IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& model, I32 subMeshIndex);
        inline void _DrawMeshInstanced(IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& model, I32 instanceCount);
        inline void _CopyTexture(ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip);
        inline void _RenderCubemap(ICubemap* cubemap, const MaterialPtr& material, U32 dstMip);
        inline void _Blit(const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material);
        inline void _DrawFullScreenQuad(const MaterialPtr& material, const ViewportRect& viewport);

        //----------------------------------------------------------------------
        void _SetGPUDescription();
        void _CreateRequiredUniformBuffersFromFile(const String& engineVS, const String& engineFS);
        void _CreateCubeMesh();
        void _SetLimits();
        void _CreateFakeShadowMaps();

        void _FlushLightBuffer();
        void _ExecuteCommandBuffer(const CommandBuffer& cmd);
        bool _UpdateGlobalBuffer(StringID name, const void* data);

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void OnWindowSizeChanged(U16 w, U16 h) override;
        void _VSyncChanged(bool b) override;

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

        NULL_COPY_AND_ASSIGN(VkRenderer)
    };




} // End namespaces