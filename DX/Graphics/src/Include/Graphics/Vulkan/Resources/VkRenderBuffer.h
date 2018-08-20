#pragma once
/**********************************************************************
    class: RenderBuffer

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_renderBuffer.hpp"

namespace Graphics { namespace Vulkan {

    class RenderTexture;

    //**********************************************************************
    class RenderBuffer : public IRenderBuffer
    {
        friend class RenderTexture;
    public:
        RenderBuffer() = default;
        ~RenderBuffer() { _DestroyBufferAndViews(); }

        //----------------------------------------------------------------------
        // IRenderBuffer Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, TextureFormat format, SamplingDescription samplingDesc) override;
        void create(U32 width, U32 height, DepthFormat format, SamplingDescription samplingDesc) override;
        void clearColor(Color color) override;
        void clearDepthStencil(F32 depth, U8 stencil) override;
        void recreate(U32 w, U32 h) override;
        void recreate(U32 w, U32 h, SamplingDescription sd) override;
        void recreate(Graphics::TextureFormat format) override;
        void recreate(Graphics::DepthFormat format) override;
        void bindForRendering() override;

        //----------------------------------------------------------------------
        // D3D11ITexture Interface
        //----------------------------------------------------------------------
        void bind(ShaderType shaderType, U32 slot) override;

    private:
        bool m_resolved = false;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override {}

        //----------------------------------------------------------------------
        void _CreateColorBufferAndViews();
        void _CreateDepthBufferAndViews();
        void _CreateShaderResourceView();
        void _DestroyBufferAndViews();
        inline void _ClearResolvedFlag() { m_resolved = false; }

        NULL_COPY_AND_ASSIGN(RenderBuffer)
    };

} } // End namespaces