#pragma once
/**********************************************************************
    class: RenderTexture

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_render_texture.h"
#include "Vulkan/Vulkan.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class RenderTexture : public IRenderTexture
    {
    public:
        RenderTexture() = default;
        ~RenderTexture() { _DestroyFramebuffers(); };

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void create(const RenderBufferPtr& colorBuffer, const RenderBufferPtr& depthBuffer) override;
        void create(const ArrayList<RenderBufferPtr>& colorBuffers, const ArrayList<RenderBufferPtr>& depthBuffers) override;
        void bindForRendering(U64 frameIndex) override;
        void clear(Color color, F32 depth, U8 stencil) override;
        void clearDepthStencil(F32 depth, U8 stencil) override;
        void recreate(U32 w, U32 h);
        void recreate(MSAASamples samples) override;
        void recreate(U32 w, U32 h, MSAASamples samples) override;
        void recreate(Graphics::TextureFormat format) override;
        U64* getNativeTexturePtr() const override { return nullptr; }

    private:
        ArrayList<Framebuffer> m_fbos;

        void _CreateFramebuffers();
        void _DestroyFramebuffers();

        NULL_COPY_AND_ASSIGN(RenderTexture)
    };

} } // End namespaces