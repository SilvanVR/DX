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
#include "D3D11Texture.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class RenderTexture : public IRenderTexture, public D3D11Texture
    {
        static const I32 NUM_BUFFERS = 2; // Number of render-buffers

    public:
        RenderTexture() = default;
        ~RenderTexture();

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override {}

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 depth, TextureFormat format) override;
        void clear(Color color, F32 depth, U8 stencil) override;
        void bindForRendering() override;

        //----------------------------------------------------------------------
        // D3D11Texture Interface
        //----------------------------------------------------------------------
        void bind(U32 slot) override;

    private:
        struct
        {
            ID3D11Texture2D*            m_pRenderTexture        = nullptr;
            ID3D11ShaderResourceView*   m_pRenderTextureView    = nullptr;
            ID3D11RenderTargetView*     m_pRenderTargetView     = nullptr;

            ID3D11Texture2D*            m_pDepthStencilBuffer   = nullptr;
            ID3D11DepthStencilView*     m_pDepthStencilView     = nullptr;
        } m_buffers[NUM_BUFFERS];

        ID3D11SamplerState*             m_pSampleState          = nullptr;

        // Ping pong framebuffer index
        I32 m_index = 0;

        //----------------------------------------------------------------------
        void _CreateTexture(I32 index);
        void _CreateViews(I32 index);
        void _CreateDepthBuffer(I32 index);
        void _CreateSampler();

        inline I32 _PreviousBufferIndex();

        //----------------------------------------------------------------------
        RenderTexture(const RenderTexture& other)               = delete;
        RenderTexture& operator = (const RenderTexture& other)  = delete;
        RenderTexture(RenderTexture&& other)                    = delete;
        RenderTexture& operator = (RenderTexture&& other)       = delete;
    };

} } // End namespaces