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
    void RenderBuffer::create( U32 width, U32 height, TextureFormat format, MSAASamples samples )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );
        m_isDepthBuffer = false;
        m_sampleCount = samples;

        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
        _CreateFramebuffer( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::create( U32 width, U32 height, DepthFormat format, MSAASamples samples )
    {
        ITexture::_Init( TextureDimension::Tex2D, width, height, TextureFormat::Depth );
        m_isDepthBuffer = true;
        m_depthFormat = format;
        m_sampleCount = samples;

        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
        _CreateFramebuffer( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( U32 w, U32 h )
    {
        recreate( w, h, m_sampleCount );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( U32 w, U32 h, MSAASamples samples )
    {
        m_width = w;
        m_height = h;
        m_sampleCount = samples;
        _DestroyFramebuffer( m_isDepthBuffer );
        _CreateFramebuffer( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( Graphics::TextureFormat format )
    {
        ASSERT( isColorBuffer() && "Renderbuffer is not a color buffer!" );
        m_format = format;
        _DestroyFramebuffer( m_isDepthBuffer );
        _CreateFramebuffer( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::recreate( Graphics::DepthFormat format )
    {
        ASSERT( isDepthBuffer() && "Renderbuffer is not a depth buffer!");
        m_depthFormat = format;
        _DestroyFramebuffer( m_isDepthBuffer );
        _CreateFramebuffer( m_isDepthBuffer );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::bindForRendering()
    {
        g_vulkan.ctx.OMSetRenderTarget( isMultisampled() ? m_framebufferMS.fbo : m_framebuffer.fbo, [this] { this->_ResolveImage(); } );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::bind( const ShaderResourceDeclaration& res )
    {
        // Bind image
        g_vulkan.ctx.SetImage( m_framebuffer.view, getSampler(), res.getBindingSet(), res.getBindingSlot() );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearColor( Color color )
    {
        ASSERT( not isDepthBuffer() );
        isMultisampled() ? m_framebufferMS.fbo.setClearColor( 0, color ) : m_framebuffer.fbo.setClearColor( 0, color );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::clearDepthStencil( F32 depth, U8 stencil )
    {
        ASSERT( isDepthBuffer() );
        isMultisampled() ? m_framebufferMS.fbo.setClearDepthStencil( 0, depth, stencil ) : m_framebuffer.fbo.setClearDepthStencil( 0, depth, stencil );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderBuffer::_CreateFramebuffer( bool isDepthBuffer )
    {
        VezImageCreateInfo imageCreateInfo{};
        imageCreateInfo.imageType   = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format      = isDepthBuffer ? Utility::TranslateDepthFormat( m_depthFormat ) : Utility::TranslateTextureFormat( m_format );
        imageCreateInfo.extent      = { m_width, m_height, 1 };
        imageCreateInfo.mipLevels   = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage       = isDepthBuffer ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        imageCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (isMultisampled())
            imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        VALIDATE( vezCreateImage( g_vulkan.device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &m_framebuffer.img ) );

        VezImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.image                        = m_framebuffer.img;
        viewCreateInfo.viewType                     = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format                       = imageCreateInfo.format;
        viewCreateInfo.subresourceRange.layerCount  = VK_REMAINING_ARRAY_LAYERS;
        viewCreateInfo.subresourceRange.levelCount  = VK_REMAINING_MIP_LEVELS;
        VALIDATE( vezCreateImageView( g_vulkan.device, &viewCreateInfo, &m_framebuffer.view ) );

        m_framebuffer.fbo.create( m_width, m_height, 1, &m_framebuffer.view, VK_SAMPLE_COUNT_1_BIT );

        // If multisampling was requested create an additional buffer in which we render, but have to resolve it before using it in a shader
        if (isMultisampled())
        {
            imageCreateInfo.samples = Utility::TranslateSampleCount( m_sampleCount );
            imageCreateInfo.usage = isDepthBuffer ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            VALIDATE( vezCreateImage( g_vulkan.device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &m_framebufferMS.img ) );

            viewCreateInfo.image = m_framebufferMS.img;
            VALIDATE( vezCreateImageView( g_vulkan.device, &viewCreateInfo, &m_framebufferMS.view ) );

            m_framebufferMS.fbo.create( m_width, m_height, 1, &m_framebufferMS.view, imageCreateInfo.samples );
            m_framebufferMS.fbo.setEndRenderPassCallback([this] {
                this->_ResolveImage();
            });
        }
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_DestroyFramebuffer( bool isDepthBuffer )
    {
        vezDeviceWaitIdle( g_vulkan.device );
        vezDestroyImage( g_vulkan.device, m_framebuffer.img );
        vezDestroyImageView( g_vulkan.device, m_framebuffer.view );
        m_framebuffer.fbo.destroy();

        vezDestroyImage( g_vulkan.device, m_framebufferMS.img );
        vezDestroyImageView( g_vulkan.device, m_framebufferMS.view );
        m_framebufferMS.fbo.destroy();
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_PushToGPU() 
    { 
        LOG_WARN_RENDERING( "VkRenderBuffer:_PushToGPU() called, which should not have happened!" );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_GenerateMips()
    {
        LOG_WARN_RENDERING( "VkRenderBuffer: Mip-Map generation not supported on RenderBuffers!" );
        //g_vulkan.ctx.GenerateMips( m_framebuffer.img, m_width, m_height, m_mipCount );
    }

    //----------------------------------------------------------------------
    void RenderBuffer::_ResolveImage()
    {
        ASSERT ( isMultisampled() );

        if ( isDepthBuffer() )
        {
            static bool once = true;
            if (once)
            {
                LOG_WARN_RENDERING( "VkRenderBuffer: _ResolveImage() on depth-buffer which is not supported (yet)." );
                once = false;
            }
            return;
        }

        g_vulkan.ctx.ResolveImage( m_framebufferMS.img, m_framebuffer.img, { m_width, m_height });
    }

} } // End namespaces