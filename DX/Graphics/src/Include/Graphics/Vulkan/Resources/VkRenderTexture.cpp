#include "VkRenderTexture.h"
/**********************************************************************
    class: RenderTexture

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "VkRenderBuffer.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderTexture::create( const RenderBufferPtr& colorBuffer, const RenderBufferPtr& depthBuffer )
    {
        IRenderTexture::create( colorBuffer, depthBuffer );
        _CreateFramebuffers();
    }

    //----------------------------------------------------------------------
    void RenderTexture::create( const ArrayList<RenderBufferPtr>& colorBuffers, const ArrayList<RenderBufferPtr>& depthBuffers )
    {
        IRenderTexture::create( colorBuffers, depthBuffers );
        _CreateFramebuffers();
    }

    //----------------------------------------------------------------------
    void RenderTexture::bindForRendering( U64 frameIndex )
    {
        // This is a little hack in order to advance the buffer index only once, because it can be bound several times per frame
        if (m_curFrameIndex != frameIndex)
        {
            m_bufferIndex = (m_bufferIndex + 1) % m_renderBuffers.size();
            m_curFrameIndex = frameIndex;
        }

        // Bind buffer
        g_vulkan.ctx.OMSetRenderTarget( m_fbos[m_bufferIndex] );
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( U32 w, U32 h )
    {
        _DestroyFramebuffers();
        IRenderTexture::recreate( w, h );
        _CreateFramebuffers();
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( SamplingDescription samplingDesc )
    {
        _DestroyFramebuffers();
        IRenderTexture::recreate( samplingDesc );
        _CreateFramebuffers();
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( U32 w, U32 h, SamplingDescription samplingDesc )
    {
        _DestroyFramebuffers();
        IRenderTexture::recreate( w, h, samplingDesc );
        _CreateFramebuffers();
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( Graphics::TextureFormat format )
    {
        _DestroyFramebuffers();
        IRenderTexture::recreate( format );
        _CreateFramebuffers();
    }

    //----------------------------------------------------------------------
    void RenderTexture::clear( Color color, F32 depth, U8 stencil )
    {
        IRenderTexture::clear( color, depth, stencil );
        for (auto& fbo : m_fbos)
        {
            fbo.setClearColor( 0, color );
            if ( hasDepthBuffer() )
                fbo.setClearDepthStencil( 1, depth, stencil );
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::clearDepthStencil( F32 depth, U8 stencil )
    {
        IRenderTexture::clearDepthStencil( depth, stencil );
        for (auto& fbo : m_fbos)
            fbo.setClearDepthStencil( hasColorBuffer() ? 1 : 0, depth, stencil );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderTexture::_CreateFramebuffers()
    {
        m_fbos.resize( m_renderBuffers.size() );
        for (I32 i = 0; i < m_fbos.size(); i++)
        {
            U32 count = 0;
            VkImageView imageViews[2];
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
            if (hasColorBuffer())
            {
                auto colorBuffer = reinterpret_cast<Vulkan::RenderBuffer*>( m_renderBuffers[i].m_colorBuffer.get() );
                if (colorBuffer->isMultisampled())
                {
                    imageViews[count++] = colorBuffer->m_framebufferMS.view;
                    samples = colorBuffer->m_framebufferMS.fbo.samples;
                }
                else
                {
                    imageViews[count++] = colorBuffer->m_framebuffer.view;
                }
            }
            if (hasDepthBuffer())
            {
                auto depthBuffer = reinterpret_cast<Vulkan::RenderBuffer*>( m_renderBuffers[i].m_depthBuffer.get() );
                if (depthBuffer->isMultisampled())
                {
                    imageViews[count++] = depthBuffer->m_framebufferMS.view;
                    samples = depthBuffer->m_framebufferMS.fbo.samples;
                }
                else
                {
                    imageViews[count++] = depthBuffer->m_framebuffer.view;
                }
            }
            ASSERT( count > 0 );
            m_fbos[i].create( getWidth(), getHeight(), count, imageViews, samples );

            if (samples != VK_SAMPLE_COUNT_1_BIT)
            {
                m_fbos[i].setEndRenderPassCallback([this, i] {
                    auto colorBuffer = reinterpret_cast<Vulkan::RenderBuffer*>( m_renderBuffers[i].m_colorBuffer.get() );
                    auto depthBuffer = reinterpret_cast<Vulkan::RenderBuffer*>( m_renderBuffers[i].m_depthBuffer.get() );
                    if (colorBuffer) colorBuffer->_ResolveImage();
                    if (depthBuffer) depthBuffer->_ResolveImage();
                });
            }
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::_DestroyFramebuffers()
    {
        for (auto& fbo : m_fbos)
            fbo.destroy();
        m_fbos.clear();
    }

} } // End namespaces