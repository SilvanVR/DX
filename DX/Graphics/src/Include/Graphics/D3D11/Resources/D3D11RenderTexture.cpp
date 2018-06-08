#include "D3D11RenderTexture.h"
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "../D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    void RenderTexture::create( U32 width, U32 height, U32 depth, TextureFormat format, U32 numBuffers, SamplingDescription samplingDescription )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );
        m_depth = depth;
        m_samplingDescription = samplingDescription;

        m_buffers.resize( numBuffers );
        _CreateBufferAndViews();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( U32 w, U32 h )
    {
        recreate( w, h, m_samplingDescription );
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( U32 w, U32 h, SamplingDescription samplingDescription )
    {
        m_width = w;
        m_height = h;
        m_samplingDescription = samplingDescription;
        _DestroyBufferAndViews();
        _CreateBufferAndViews();
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( Graphics::TextureFormat format )
    {
        m_format = format;
        _DestroyBufferAndViews();
        _CreateBufferAndViews();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderTexture::bind( ShaderType shaderType, U32 slot )
    {
        // Bind previous rendered buffer
        I32 index = _PreviousBufferIndex();

        switch (shaderType)
        {
        case ShaderType::Vertex:
            g_pImmediateContext->VSSetShaderResources( slot, 1, &m_buffers[index].pRenderTextureView );
            g_pImmediateContext->VSSetSamplers( slot, 1, &m_pSampleState );
            break;
        case ShaderType::Fragment:
            g_pImmediateContext->PSSetShaderResources( slot, 1, &m_buffers[index].pRenderTextureView );
            g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );
            break;
        default:
            ASSERT( false );
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::bindForRendering()
    {
        // Bind next buffer
        m_index = (m_index + 1) % m_buffers.size();
        g_pImmediateContext->OMSetRenderTargets( 1, &m_buffers[m_index].pRenderTargetView, m_depth == 0 ? nullptr : m_buffers[m_index].pDepthStencilView );
    }

    //----------------------------------------------------------------------
    void RenderTexture::clear( Color color, F32 depth, U8 stencil )
    {
        g_pImmediateContext->ClearRenderTargetView( m_buffers[m_index].pRenderTargetView, color.normalized().data() );
        if (m_depth > 0)
            g_pImmediateContext->ClearDepthStencilView( m_buffers[m_index].pDepthStencilView, (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL), depth, stencil);
    }

    //----------------------------------------------------------------------
    void RenderTexture::clearDepthStencil( F32 depth, U8 stencil )
    {
        ASSERT( m_depth > 0 && "Render-Texture has no depth-buffer! Can't clear it!" );
        g_pImmediateContext->ClearDepthStencilView( m_buffers[m_index].pDepthStencilView, (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL), depth, stencil );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    I32 RenderTexture::_PreviousBufferIndex()
    {
        I32 index = (m_index - 1);
        if (index < 0)
            index += (I32)m_buffers.size();
        return index;
    }

    //----------------------------------------------------------------------
    void RenderTexture::_CreateTexture( I32 index )
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.Width               = static_cast<UINT>( m_width * m_scaleFactor );
        textureDesc.Height              = static_cast<UINT>( m_height * m_scaleFactor );
        textureDesc.MipLevels           = 1;
        textureDesc.ArraySize           = 1;
        textureDesc.Format              = Utility::TranslateTextureFormat( m_format );
        textureDesc.SampleDesc          = { m_samplingDescription.count, m_samplingDescription.quality };
        textureDesc.Usage               = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags      = 0;
        textureDesc.MiscFlags           = 0;

        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_buffers[index].pRenderTexture) );
    }

    //----------------------------------------------------------------------
    void RenderTexture::_CreateViews( I32 index )
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = Utility::TranslateTextureFormat( m_format );
        srvDesc.ViewDimension = m_samplingDescription.count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR( g_pDevice->CreateShaderResourceView( m_buffers[index].pRenderTexture, &srvDesc, &m_buffers[index].pRenderTextureView ) );

        HR( g_pDevice->CreateRenderTargetView( m_buffers[index].pRenderTexture, NULL, &m_buffers[index].pRenderTargetView ) );
    }

    //----------------------------------------------------------------------
    void RenderTexture::_CreateDepthBuffer( I32 index )
    {
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width          = static_cast<UINT>( m_width * m_scaleFactor );
        depthStencilDesc.Height         = static_cast<UINT>( m_height * m_scaleFactor );
        depthStencilDesc.MipLevels      = 1;
        depthStencilDesc.ArraySize      = 1;
        depthStencilDesc.Format         = Utility::TranslateDepthFormat( m_depth );
        depthStencilDesc.SampleDesc     = { m_samplingDescription.count, m_samplingDescription.quality };
        depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags      = 0;

        HR( g_pDevice->CreateTexture2D( &depthStencilDesc, NULL, &m_buffers[index].pDepthStencilBuffer) );
        HR( g_pDevice->CreateDepthStencilView( m_buffers[index].pDepthStencilBuffer, NULL, &m_buffers[index].pDepthStencilView ) );
    }

    //----------------------------------------------------------------------
    void RenderTexture::_CreateBufferAndViews()
    {
        for (I32 i = 0; i < m_buffers.size(); i++)
        {
            _CreateTexture( i );
            _CreateViews( i );
            if (m_depth > 0)
                _CreateDepthBuffer( i );
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::_DestroyBufferAndViews()
    {
        for (I32 i = 0; i < m_buffers.size(); i++)
        {
            SAFE_RELEASE( m_buffers[i].pRenderTexture );
            SAFE_RELEASE( m_buffers[i].pRenderTextureView );
            SAFE_RELEASE( m_buffers[i].pRenderTargetView );
            SAFE_RELEASE( m_buffers[i].pDepthStencilBuffer );
            SAFE_RELEASE( m_buffers[i].pDepthStencilView );
        }
    }

} } // End namespaces