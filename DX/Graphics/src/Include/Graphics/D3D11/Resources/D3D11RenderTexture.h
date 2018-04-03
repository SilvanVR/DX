#pragma once
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.h)

    author: S. Hau
    date: March 24, 2018

    D3D11 implementation of a render-texture. It consists of more than 
    one buffer, so it can be bound as a srv and to the output-merger
    at the same time. Internally always the previous rendered buffer
    will be used when bound as a srv.
**********************************************************************/

#include "i_render_texture.hpp"
#include "../D3D11.hpp"
#include "D3D11IBindableTexture.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class RenderTexture : public Graphics::IRenderTexture, public D3D11::IBindableTexture
    {
        static const I32 NUM_BUFFERS = 2; // Number of render-buffers

    public:
        RenderTexture() = default;
        ~RenderTexture();

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 depth, TextureFormat format) override;
        void clear(Color color, F32 depth, U8 stencil) override;
        void bindForRendering() override;

        //----------------------------------------------------------------------
        // D3D11ITexture Interface
        //----------------------------------------------------------------------
        void bind(U32 slot) override;
        ID3D11Texture2D* getD3D11Texture() override { return m_buffers[_PreviousBufferIndex()].pRenderTexture; }

    private:
        struct
        {
            ID3D11Texture2D*            pRenderTexture        = nullptr;
            ID3D11ShaderResourceView*   pRenderTextureView    = nullptr;
            ID3D11RenderTargetView*     pRenderTargetView     = nullptr;

            ID3D11Texture2D*            pDepthStencilBuffer   = nullptr;
            ID3D11DepthStencilView*     pDepthStencilView     = nullptr;
        } m_buffers[NUM_BUFFERS];

        // Ping pong framebuffer index
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