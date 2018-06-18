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

        _SetMultisampleDesc( samplingDesc );

        _CreateColorBufferAndViews();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, DepthFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, TextureFormat::Depth );
        m_isDepthBuffer = true;
        m_depthFormat = format;

        _SetMultisampleDesc( samplingDesc );

        _CreateDepthBufferAndViews();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( U32 w, U32 h )
    {
        recreate( w, h, m_samplingDescription );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( U32 w, U32 h, SamplingDescription samplingDesc )
    {
        m_width = w;
        m_height = h;

        _SetMultisampleDesc( samplingDesc );

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
        _ClearResolvedFlag();
        if ( isDepthBuffer() )
            g_pImmediateContext->OMSetRenderTargets( 0, NULL, m_pDepthStencilView );
        else
            g_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::bind( ShaderType shaderType, U32 slot )
    {
        // If the renderbuffer is multisampled itself, we must resolve it to the non-multisampled buffer and bind that to the shader then
        if ( isMultisampled() )
        {
            if ( isDepthBuffer() )
            {
                LOG_WARN_RENDERING( "D3D11RenderBuffer::bind(): Trying to bind a multisampled depth-buffer, but must be resolved first! This is not supported yet!" );
            }
            else
            {
                if (not m_resolved)
                {
                    g_pImmediateContext->ResolveSubresource( m_pTexture, 0, m_pRenderBufferMS, 0, Utility::TranslateTextureFormat( m_format ) );
                    m_resolved = true;
                }
            }
        }

        switch (shaderType)
        {
        case ShaderType::Vertex:
            g_pImmediateContext->VSSetShaderResources( slot, 1, &m_pTextureView );
            g_pImmediateContext->VSSetSamplers( slot, 1, &m_pSampleState );
            break;
        case ShaderType::Fragment:
            g_pImmediateContext->PSSetShaderResources( slot, 1, &m_pTextureView );
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
        textureDesc.Usage               = D3D11_USAGE_DEFAULT;
        textureDesc.CPUAccessFlags      = 0;
        textureDesc.MiscFlags           = 0;
        textureDesc.SampleDesc          = { 1, 0 };
        textureDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

        // Create always a non multisampled render buffer
        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pTexture ) );

        // If multisampling was requested create an additional buffer in which we render, but have to resolve it before using it in a shader
        if ( isMultisampled() )
        {
            textureDesc.SampleDesc      = { m_samplingDescription.count, m_samplingDescription.quality };
            textureDesc.BindFlags       = D3D11_BIND_RENDER_TARGET;
            HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pRenderBufferMS ) );
            HR( g_pDevice->CreateRenderTargetView( m_pRenderBufferMS, NULL, &m_pRenderTargetView ) );
        }
        else
        {
            // Just use the non multisampled render buffer as the render target
            HR( g_pDevice->CreateRenderTargetView( m_pTexture, NULL, &m_pRenderTargetView ) );
        }

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

        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pTexture ) );

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Flags = 0;
        dsvDesc.Format = Utility::TranslateDepthFormat( m_depthFormat );
        dsvDesc.ViewDimension = m_samplingDescription.count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

        HR( g_pDevice->CreateDepthStencilView( m_pTexture, &dsvDesc, &m_pDepthStencilView ) );

        _CreateShaderResourceView();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateShaderResourceView()
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

        if ( isDepthBuffer() )
        {
            switch (m_depthFormat)
            {
            case DepthFormat::D16:   srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;
            case DepthFormat::D24S8: srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
            case DepthFormat::D32:   srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
            ASSERT( false && "Ooops! Something is wrong here!" );
            }
            srvDesc.ViewDimension = isMultisampled() ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        }
        else
        {
            srvDesc.Format = Utility::TranslateTextureFormat( m_format );
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        }

        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_DestroyBufferAndViews()
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
        SAFE_RELEASE( m_pRenderBufferMS );
        SAFE_RELEASE( m_pShaderBufferViewMS );

        if ( isDepthBuffer() )
        {
            SAFE_RELEASE( m_pDepthStencilView );
        }
        else
        {
            SAFE_RELEASE( m_pRenderTargetView );
        }
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_SetMultisampleDesc( SamplingDescription samplingDesc )
    {
        m_samplingDescription = samplingDesc;

        DXGI_FORMAT format = isDepthBuffer() ? Utility::TranslateDepthFormatSRV( m_depthFormat ) : Utility::TranslateTextureFormat( m_format );
        if ( not Utility::MSAASamplesSupported( format, m_samplingDescription.count ) )
        { 
            LOG_WARN_RENDERING( "D3D11: MSAA count (" + TS( m_samplingDescription.count ) + ") for render-buffer is not supported. Count will be set to 1." );
            m_samplingDescription.count = 1;
        }
    }

} } // End namespaces