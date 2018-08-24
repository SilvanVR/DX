#include "VkRenderBuffer.h"
/**********************************************************************
    class: RenderBuffer

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "Vulkan/VkUtility.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, TextureFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );
        m_isDepthBuffer = false;
        m_samplingDescription = samplingDesc;

        _CreateImage( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, DepthFormat format, SamplingDescription samplingDesc )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, TextureFormat::Depth );
        m_isDepthBuffer = true;
        m_depthFormat = format;
        m_samplingDescription = samplingDesc;

        _CreateImage( m_isDepthBuffer );
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
        _DestroyBuffers( m_isDepthBuffer );
        _CreateImage( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( Graphics::TextureFormat format )
    {
        ASSERT( isColorBuffer() && "Renderbuffer is not a color buffer!" );
        m_format = format;
        _DestroyBuffers( m_isDepthBuffer );
        _CreateImage( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( Graphics::DepthFormat format )
    {
        ASSERT( isDepthBuffer() && "Renderbuffer is not a depth buffer!");
        m_depthFormat = format;
        _DestroyBuffers( m_isDepthBuffer );
        _CreateImage( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::bindForRendering()
    {
        _ClearResolvedFlag();
        if ( isDepthBuffer() )
            g_vulkan.ctx.OMSetRenderTarget( VK_NULL_HANDLE, &m_imageView );
        else
            g_vulkan.ctx.OMSetRenderTarget( &m_imageView, VK_NULL_HANDLE );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::bind( ShaderType shaderType, U32 set )
    {
        // If the renderbuffer is multisampled itself, we must resolve it to the non-multisampled buffer and bind that to the shader then
        if ( isMultisampled() )
        {
            if (not m_resolved)
            {
                g_vulkan.ctx.ResolveImage( &m_colorImageMS, &m_colorImage );
                m_resolved = true;
            }
        }

        // Bind descriptor-set containing this image

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
        g_vulkan.ctx.SetClearColor( color );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearDepthStencil( F32 depth, U8 stencil )
    {
        ASSERT( isDepthBuffer() );
        g_vulkan.ctx.SetClearDepthStencil( depth, stencil );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateImage( bool isDepthBuffer )
    {
        // If multisampling was requested create an additional buffer in which we render, but have to resolve it before using it in a shader
        if (isDepthBuffer)
        {
            auto format = Utility::TranslateDepthFormat( m_depthFormat );
            m_depthImage.create( m_width, m_height, format, VK_SAMPLE_COUNT_1_BIT );
            if (isMultisampled())
                m_depthImageMS.create( m_width, m_height, format, Utility::TranslateSampleCount( m_samplingDescription ) );
            m_imageView.create( m_depthImage );
        }
        else
        {
            auto format = Utility::TranslateTextureFormat( m_format );
            m_colorImage.create( m_width, m_height, format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY );
            if (isMultisampled())
                m_colorImageMS.create( m_width, m_height, format, Utility::TranslateSampleCount( m_samplingDescription ),
                                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY );
            m_imageView.create( m_colorImage );
        }
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_DestroyBuffers( bool isDepthBuffer )
    {
        m_imageView.release();
        if (isDepthBuffer)
        {
            m_depthImage.release();
            m_depthImageMS.release();
        }
        else
        {
            m_colorImage.release();
            m_colorImageMS.release();
        }
    }

} } // End namespaces