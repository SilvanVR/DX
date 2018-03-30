#pragma once
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.h)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "i_render_texture.hpp"
#include "../D3D11.hpp"
#include "D3D11Texture.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class RenderTexture : public IRenderTexture, public D3D11Texture
    {
    public:
        RenderTexture(U32 width, U32 height, U32 depth, TextureFormat format);
        ~RenderTexture();

        //----------------------------------------------------------------------
        // D3D11Texture Interface
        //----------------------------------------------------------------------
        void bind(U32 slot) override;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override {}
        void apply() override {}

        void bindForRendering();

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void clear(Color color, F32 depth, U8 stencil) override;

    private:
        static const I32 NUM_BUFFERS = 2;

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
        void _CreateRenderTargetAndView(I32 index);
        void _CreateDepthBuffer(I32 index);
        void _CreateSampler();

        //----------------------------------------------------------------------
        RenderTexture(const RenderTexture& other)               = delete;
        RenderTexture& operator = (const RenderTexture& other)  = delete;
        RenderTexture(RenderTexture&& other)                    = delete;
        RenderTexture& operator = (RenderTexture&& other)       = delete;
    };

} } // End namespaces