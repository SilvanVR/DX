#pragma once
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.h)

    author: S. Hau
    date: March 24, 2018

    D3D11 implementation of a render-texture. It can consist of more than 
    one buffer, so it can be bound as a srv and to the output-merger
    at the same time.
**********************************************************************/

#include "i_render_texture.hpp"
#include "../D3D11.hpp"
#include "D3D11IBindableTexture.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class RenderTexture : public Graphics::IRenderTexture, public D3D11::IBindableTexture
    {
    public:
        RenderTexture() = default;
        ~RenderTexture();

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 depth, TextureFormat format, U32 numBuffers) override;
        void clear(Color color, F32 depth, U8 stencil) override;
        void clearDepthStencil(F32 depth, U8 stencil) override;
        void bindForRendering() override;

        //----------------------------------------------------------------------
        // D3D11ITexture Interface
        //----------------------------------------------------------------------
        void bind(ShaderType shaderType, U32 slot) override;
        ID3D11Texture2D* getD3D11Texture() override { return m_buffers[m_index].pRenderTexture; }

    private:
        struct RenderBuffer
        {
            ID3D11Texture2D*            pRenderTexture        = nullptr;
            ID3D11ShaderResourceView*   pRenderTextureView    = nullptr;
            ID3D11RenderTargetView*     pRenderTargetView     = nullptr;

            ID3D11Texture2D*            pDepthStencilBuffer   = nullptr;
            ID3D11DepthStencilView*     pDepthStencilView     = nullptr;
        };

        ArrayList<RenderBuffer> m_buffers;

        // Framebuffer index
        I32 m_index = 0;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }

        //----------------------------------------------------------------------
        void _CreateTexture(I32 index);
        void _CreateViews(I32 index);
        void _CreateDepthBuffer(I32 index);

        inline I32 _PreviousBufferIndex();

        //----------------------------------------------------------------------
        RenderTexture(const RenderTexture& other)               = delete;
        RenderTexture& operator = (const RenderTexture& other)  = delete;
        RenderTexture(RenderTexture&& other)                    = delete;
        RenderTexture& operator = (RenderTexture&& other)       = delete;
    };

} } // End namespaces