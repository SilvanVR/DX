#include "VkRenderBuffer.h"
/**********************************************************************
    class: RenderBuffer

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, TextureFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );
        m_isDepthBuffer = false;

        m_samplingDescription = samplingDesc;

        _CreateColorBufferAndViews();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, DepthFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, TextureFormat::Depth );
        m_isDepthBuffer = true;
        m_depthFormat = format;
        m_samplingDescription = samplingDesc;

        _CreateDepthBufferAndViews();
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
        m_samplingDescription = samplingDesc;

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
        //if ( isDepthBuffer() )
        //    g_pImmediateContext->OMSetRenderTargets( 0, NULL, m_pDepthStencilView );
        //else
        //    g_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::bind( ShaderType shaderType, U32 slot )
    {
        //// If the renderbuffer is multisampled itself, we must resolve it to the non-multisampled buffer and bind that to the shader then
        //if ( isMultisampled() )
        //{
        //    if ( isDepthBuffer() )
        //    {
        //        LOG_WARN_RENDERING( "D3D11RenderBuffer::bind(): Trying to bind a multisampled depth-buffer, but must be resolved first! This is not supported yet!" );
        //    }
        //    else
        //    {
        //        if (not m_resolved)
        //        {
        //            g_pImmediateContext->ResolveSubresource( m_pTexture, 0, m_pRenderBufferMS, 0, Utility::TranslateTextureFormat( m_format ) );
        //            m_resolved = true;
        //        }
        //    }
        //}

        //switch (shaderType)
        //{
        //case ShaderType::Vertex:
        //    g_pImmediateContext->VSSetShaderResources( slot, 1, &m_pTextureView );
        //    g_pImmediateContext->VSSetSamplers( slot, 1, &m_pSampleState );
        //    break;
        //case ShaderType::Fragment:
        //    g_pImmediateContext->PSSetShaderResources( slot, 1, &m_pTextureView );
        //    g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );
        //    break;
        //default:
        //    ASSERT( false );
        //}
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearColor( Color color )
    {
        ASSERT( not isDepthBuffer() );
        //g_pImmediateContext->ClearRenderTargetView( m_pRenderTargetView, color.normalized().data() );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearDepthStencil( F32 depth, U8 stencil )
    {
        ASSERT( isDepthBuffer() );
        //g_pImmediateContext->ClearDepthStencilView( m_pDepthStencilView, (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL), depth, stencil );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateColorBufferAndViews()
    {
        // If multisampling was requested create an additional buffer in which we render, but have to resolve it before using it in a shader
        if ( isMultisampled() )
        {
        }
        else
        {
            // Just use the non multisampled render buffer as the render target

        }

        _CreateShaderResourceView();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateDepthBufferAndViews()
    {
        _CreateShaderResourceView();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateShaderResourceView()
    {
        //D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

        //if ( isDepthBuffer() )
        //{
        //    switch (m_depthFormat)
        //    {
        //    case DepthFormat::D16:   srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;
        //    case DepthFormat::D24S8: srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
        //    case DepthFormat::D32:   srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
        //    ASSERT( false && "Ooops! Something is wrong here!" );
        //    }
        //    srvDesc.ViewDimension = isMultisampled() ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        //}
        //else
        //{
        //    srvDesc.Format = Utility::TranslateTextureFormat( m_format );
        //    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        //}

        //srvDesc.Texture2D.MostDetailedMip = 0;
        //srvDesc.Texture2D.MipLevels = -1;

        //HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_DestroyBufferAndViews()
    {
        if ( isDepthBuffer() )
        {

        }
        else
        {

        }
    }

} } // End namespaces