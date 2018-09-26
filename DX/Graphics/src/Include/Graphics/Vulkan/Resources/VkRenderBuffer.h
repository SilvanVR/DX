#pragma once
/**********************************************************************
    class: RenderBuffer

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_renderBuffer.hpp"
#include "Vulkan/Vulkan.hpp"
#include "../Resources/VkBindableTexture.h"

namespace Graphics { namespace Vulkan {

    class RenderTexture;

    //**********************************************************************
    class RenderBuffer : public IRenderBuffer, public IBindableTexture
    {
        friend class RenderTexture;
    public:
        RenderBuffer() {}
        ~RenderBuffer() { _DestroyFramebuffer(isDepthBuffer()); }

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void bind(const ShaderResourceDeclaration& res) override;
        U64* getNativeTexturePtr() const override { return reinterpret_cast<U64*>(m_framebuffer.img); }

        //----------------------------------------------------------------------
        // IRenderBuffer Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, TextureFormat format, MSAASamples samples) override;
        void clearColor(Color color) override;
        void clearDepthStencil(F32 depth, U8 stencil) override;
        void recreate(U32 w, U32 h) override;
        void recreate(U32 w, U32 h, MSAASamples samples) override;
        void recreate(Graphics::TextureFormat format) override;
        void bindForRendering() override;

    private:
        struct
        {
            VkImage     img = VK_NULL_HANDLE;
            VkImageView view = VK_NULL_HANDLE;
            Framebuffer fbo;
        } m_framebuffer, m_framebufferMS;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { IBindableTexture::_CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void _PushToGPU() override;
        void _GenerateMips() override;

        //----------------------------------------------------------------------
        void _CreateFramebuffer(bool isDepthBuffer);
        void _DestroyFramebuffer(bool isDepthBuffer);
        void _ResolveImage();

        NULL_COPY_AND_ASSIGN(RenderBuffer)
    };

} } // End namespaces