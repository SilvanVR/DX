#pragma once
/**********************************************************************
    class: RenderBuffer (D3D11RenderBuffer.h)

    author: S. Hau
    date: June 9, 2018

    D3D11 RenderBuffer. Can be either a color- or depthbuffer.
**********************************************************************/

#include "i_renderBuffer.hpp"
#include "D3D11/D3D11.hpp"
#include "D3D11IBindableTexture.h"

namespace Graphics { namespace D3D11 {

    class RenderTexture;

    //**********************************************************************
    class RenderBuffer : public IRenderBuffer, public D3D11::IBindableTexture
    {
        friend class D3D11::RenderTexture; // Access to pRenderBuffer
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
        ID3D11Texture2D* getD3D11Texture() override { return m_pRenderBuffer; }

    private:
        ID3D11Texture2D*            m_pRenderBuffer = nullptr;
        ID3D11ShaderResourceView*   m_pShaderBufferView = nullptr;

        union
        {
            ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
            ID3D11DepthStencilView* m_pDepthStencilView;
        };

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }

        //----------------------------------------------------------------------
        void _CreateColorBufferAndViews();
        void _CreateDepthBufferAndViews();
        void _CreateShaderResourceView();
        void _DestroyBufferAndViews();

        //----------------------------------------------------------------------
        RenderBuffer(const RenderBuffer& other)               = delete;
        RenderBuffer& operator = (const RenderBuffer& other)  = delete;
        RenderBuffer(RenderBuffer&& other)                    = delete;
        RenderBuffer& operator = (RenderBuffer&& other)       = delete;
    };

} } // End namespaces