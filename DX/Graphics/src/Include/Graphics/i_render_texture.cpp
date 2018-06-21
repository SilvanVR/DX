#include "i_render_texture.h"
/**********************************************************************
    class: RenderTexture (render_texture.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

namespace Graphics
{

    //----------------------------------------------------------------------
    void RenderTexture::setDynamicScreenScale( bool shouldScale, F32 scaleFactor ) 
    { 
        m_dynamicScale = shouldScale;
        if (scaleFactor != m_scaleFactor)
        {
            m_scaleFactor = scaleFactor;
            for (auto& buffer : m_renderBuffers)
            {
                buffer.m_colorBuffer->recreate( U32(buffer.m_colorBuffer->getWidth() * m_scaleFactor), U32(buffer.m_colorBuffer->getHeight() * m_scaleFactor) );
                if ( hasDepthBuffer() )
                    buffer.m_depthBuffer->recreate( U32(buffer.m_depthBuffer->getWidth() * m_scaleFactor), U32(buffer.m_depthBuffer->getHeight() * m_scaleFactor) );
            }
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::clear( Color color, F32 depth, U8 stencil )
    {
        m_renderBuffers[m_bufferIndex].m_colorBuffer->clearColor( color );
        if ( hasDepthBuffer() )
            m_renderBuffers[m_bufferIndex].m_depthBuffer->clearDepthStencil( depth, stencil );
    }

    //----------------------------------------------------------------------
    void RenderTexture::clearDepthStencil( F32 depth, U8 stencil )
    {
        m_renderBuffers[m_bufferIndex].m_depthBuffer->clearDepthStencil( depth, stencil );
    }

    //----------------------------------------------------------------------
    void RenderTexture::create( const RenderBufferPtr& colorBuffer, const RenderBufferPtr& depthBuffer )
    {
        m_renderBuffers.resize( 1 );
        m_renderBuffers[0].m_colorBuffer = colorBuffer;
        m_renderBuffers[0].m_depthBuffer = depthBuffer;
    }

    //----------------------------------------------------------------------
    void RenderTexture::create( const ArrayList<RenderBufferPtr>& colorBuffers, const ArrayList<RenderBufferPtr>& depthBuffers )
    {
        m_renderBuffers.resize( colorBuffers.size() );
        for (I32 i = 0; i < m_renderBuffers.size(); i++)
        {
            m_renderBuffers[i].m_colorBuffer = colorBuffers[i];
            m_renderBuffers[i].m_depthBuffer = i < depthBuffers.size() ? depthBuffers[i] : nullptr;
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( U32 w, U32 h, SamplingDescription samplingDesc )
    {
        for (auto& buffer : m_renderBuffers)
        {
            buffer.m_colorBuffer->recreate( U32(w * m_scaleFactor), U32(h * m_scaleFactor), samplingDesc );
            if ( hasDepthBuffer() )
                buffer.m_depthBuffer->recreate( U32(w * m_scaleFactor), U32(h * m_scaleFactor), samplingDesc );
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::recreate( Graphics::TextureFormat format )
    {
        for (auto& buffer : m_renderBuffers)
            buffer.m_colorBuffer->recreate( format );
    }

    //----------------------------------------------------------------------
    void RenderTexture::setFilter( TextureFilter filter )
    {
        for (auto& buffer : m_renderBuffers)
        {
            buffer.m_colorBuffer->setFilter( filter );
            if ( hasDepthBuffer() )
                buffer.m_depthBuffer->setFilter( filter );
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::setClampMode( TextureAddressMode clampMode )
    {
        for (auto& buffer : m_renderBuffers)
        {
            buffer.m_colorBuffer->setClampMode( clampMode );
            if ( hasDepthBuffer() )
                buffer.m_depthBuffer->setClampMode( clampMode );
        }
    }

    //----------------------------------------------------------------------
    void RenderTexture::setAnisoLevel( U32 level )
    {
        for (auto& buffer : m_renderBuffers)
        {
            buffer.m_colorBuffer->setAnisoLevel( level );
            if ( hasDepthBuffer() )
                buffer.m_depthBuffer->setAnisoLevel( level );
        }
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    I32 RenderTexture::_PreviousBufferIndex()
    {
        I32 index = (m_bufferIndex - 1);
        if (index < 0)
            index += (I32)m_renderBuffers.size();
        return index;
    }

}