#include "D3D11RenderBuffer.h"
/**********************************************************************
    class: RenderBuffer (D3D11RenderBuffer.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "D3D11/D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, TextureFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );
        m_isDepthBuffer = false;
        m_samplingDescription = samplingDesc;

        _CreateColorBufferAndViews();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, DepthFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, TextureFormat::Depth );
        m_isDepthBuffer = true;
        m_depthFormat = format;
        m_samplingDescription = samplingDesc;

        _CreateDepthBufferAndViews();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( U32 w, U32 h )
    {
        recreate( w, h, m_samplingDescription );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( U32 w, U32 h, SamplingDescription samplingDescription )
    {
        m_width = w;
        m_height = h;
        m_samplingDescription = samplingDescription;
        _DestroyBufferAndViews();
        isDepthBuffer() ? _CreateDepthBufferAndViews() : _CreateColorBufferAndViews();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( Graphics::TextureFormat format )
    {
        ASSERT( isColorBuffer() && "Renderbuffer is not a color buffer!" );
        m_format = format;
        _DestroyBufferAndViews();
        _CreateColorBufferAndViews();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( Graphics::DepthFormat format )
    {
        ASSERT( isDepthBuffer() && "Renderbuffer is not a depth buffer!");
        m_depthFormat = format;
        _DestroyBufferAndViews();
        _CreateDepthBufferAndViews();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::bindForRendering()
    {
        if ( isDepthBuffer() )
            g_pImmediateContext->OMSetRenderTargets( 1, NULL, m_pDepthStencilView );
        else
            g_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::bind( ShaderType shaderType, U32 slot )
    {
        switch (shaderType)
        {
        case ShaderType::Vertex:
            g_pImmediateContext->VSSetShaderResources( slot, 1, &m_pShaderBufferView );
            g_pImmediateContext->VSSetSamplers( slot, 1, &m_pSampleState );
            break;
        case ShaderType::Fragment:
            g_pImmediateContext->PSSetShaderResources( slot, 1, &m_pShaderBufferView );
            g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );
            break;
        default:
            ASSERT( false );
        }
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearColor( Color color )
    {
        ASSERT( not isDepthBuffer() );
        g_pImmediateContext->ClearRenderTargetView( m_pRenderTargetView, color.normalized().data() );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearDepthStencil( F32 depth, U8 stencil )
    {
        ASSERT( isDepthBuffer() );
        g_pImmediateContext->ClearDepthStencilView( m_pDepthStencilView, (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL), depth, stencil );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateColorBufferAndViews()
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.Width               = m_width;
        textureDesc.Height              = m_height;
        textureDesc.MipLevels           = 1;
        textureDesc.ArraySize           = 1;
        textureDesc.Format              = Utility::TranslateTextureFormat( m_format );
        textureDesc.SampleDesc          = { m_samplingDescription.count, m_samplingDescription.quality };
        textureDesc.Usage               = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags      = 0;
        textureDesc.MiscFlags           = 0;

        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pRenderBuffer) );
        HR( g_pDevice->CreateRenderTargetView( m_pRenderBuffer, NULL, &m_pRenderTargetView ) );

        _CreateShaderResourceView();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateDepthBufferAndViews()
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width          = m_width;
        textureDesc.Height         = m_height;
        textureDesc.MipLevels      = 1;
        textureDesc.ArraySize      = 1;
        textureDesc.Format         = Utility::TranslateDepthFormatSRV( m_depthFormat );
        textureDesc.SampleDesc     = { m_samplingDescription.count, m_samplingDescription.quality };
        textureDesc.Usage          = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags      = 0;

        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pRenderBuffer) );

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Flags = 0;
        dsvDesc.Format = Utility::TranslateDepthFormat( m_depthFormat );
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        HR( g_pDevice->CreateDepthStencilView( m_pRenderBuffer, &dsvDesc, &m_pDepthStencilView ) );

        _CreateShaderResourceView();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateShaderResourceView()
    {
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        if ( isDepthBuffer() )
        {
            switch (m_depthFormat)
            {
            case DepthFormat::D16:   format = DXGI_FORMAT_R16_UNORM; break;
            case DepthFormat::D24S8: format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
            case DepthFormat::D32:   format = DXGI_FORMAT_R32_FLOAT; break;
            ASSERT( false && "Ooops! Something is wrong here!" );
            }
        }
        else
        {
            format = Utility::TranslateTextureFormat( m_format );
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = format;
        srvDesc.ViewDimension = m_samplingDescription.count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR( g_pDevice->CreateShaderResourceView( m_pRenderBuffer, &srvDesc, &m_pShaderBufferView ) );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_DestroyBufferAndViews()
    {
        SAFE_RELEASE( m_pRenderBuffer );
        SAFE_RELEASE( m_pShaderBufferView );

        if ( isDepthBuffer() )
        {
            SAFE_RELEASE( m_pDepthStencilView );
        }
        else
        {
            SAFE_RELEASE( m_pRenderTargetView );
        }
    }

} } // End namespaces