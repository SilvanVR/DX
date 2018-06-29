#include "i_render_texture.h"
/**********************************************************************
    class: IRenderTexture (render_texture.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics
{

    //----------------------------------------------------------------------
    void IRenderTexture::setDynamicScreenScale( bool shouldScale, F32 scaleFactor ) 
    { 
        m_dynamicScale = shouldScale;
        if (scaleFactor != m_scaleFactor)
        {
            m_scaleFactor = scaleFactor;
            recreate( getWidth(), getHeight() );
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::clear( Color color, F32 depth, U8 stencil )
    {
        if ( hasColorBuffer() )
            m_renderBuffers[m_bufferIndex].m_colorBuffer->clearColor( color );
        if ( hasDepthBuffer() )
            m_renderBuffers[m_bufferIndex].m_depthBuffer->clearDepthStencil( depth, stencil );
    }

    //----------------------------------------------------------------------
    void IRenderTexture::clearDepthStencil( F32 depth, U8 stencil )
    {
        m_renderBuffers[m_bufferIndex].m_depthBuffer->clearDepthStencil( depth, stencil );
    }

    //----------------------------------------------------------------------
    void IRenderTexture::create( const RenderBufferPtr& colorBuffer, const RenderBufferPtr& depthBuffer )
    {
        m_renderBuffers.resize( 1 );
        m_renderBuffers[0].m_colorBuffer = colorBuffer;
        m_renderBuffers[0].m_depthBuffer = depthBuffer;
    }

    //----------------------------------------------------------------------
    void IRenderTexture::create( const ArrayList<RenderBufferPtr>& colorBuffers, const ArrayList<RenderBufferPtr>& depthBuffers )
    {
        m_renderBuffers.resize( colorBuffers.size() );
        for (I32 i = 0; i < m_renderBuffers.size(); i++)
        {
            m_renderBuffers[i].m_colorBuffer = colorBuffers[i];
            m_renderBuffers[i].m_depthBuffer = i < depthBuffers.size() ? depthBuffers[i] : nullptr;
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::recreate( U32 w, U32 h )
    {
        for (auto& buffer : m_renderBuffers)
        {
            if ( hasColorBuffer() ) buffer.m_colorBuffer->recreate( U32(w * m_scaleFactor), U32(h * m_scaleFactor) );
            if ( hasDepthBuffer() ) buffer.m_depthBuffer->recreate( U32(w * m_scaleFactor), U32(h * m_scaleFactor) );
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::recreate( U32 w, U32 h, SamplingDescription samplingDesc )
    {
        for (auto& buffer : m_renderBuffers)
        {
            if ( hasColorBuffer() ) buffer.m_colorBuffer->recreate( U32(w * m_scaleFactor), U32(h * m_scaleFactor), samplingDesc );
            if ( hasDepthBuffer() ) buffer.m_depthBuffer->recreate( U32(w * m_scaleFactor), U32(h * m_scaleFactor), samplingDesc );
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::recreate( Graphics::TextureFormat format )
    {
        for (auto& buffer : m_renderBuffers)
            buffer.m_colorBuffer->recreate( format );
    }

    //----------------------------------------------------------------------
    void IRenderTexture::setFilter( TextureFilter filter )
    {
        for (auto& buffer : m_renderBuffers)
        {
            if ( hasColorBuffer() ) buffer.m_colorBuffer->setFilter( filter );
            if ( hasDepthBuffer() ) buffer.m_depthBuffer->setFilter( filter );
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::setClampMode( TextureAddressMode clampMode )
    {
        for (auto& buffer : m_renderBuffers)
        {
            if ( hasColorBuffer() ) buffer.m_colorBuffer->setClampMode( clampMode );
            if ( hasDepthBuffer() ) buffer.m_depthBuffer->setClampMode( clampMode );
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::setAnisoLevel( U32 level )
    {
        for (auto& buffer : m_renderBuffers)
        {
            if ( hasColorBuffer() ) buffer.m_colorBuffer->setAnisoLevel( level );
            if ( hasDepthBuffer() ) buffer.m_depthBuffer->setAnisoLevel( level );
        }
    }

    //----------------------------------------------------------------------
    void IRenderTexture::bind( ShaderType shaderType, U32 bindSlot )
    {
        auto prev = _PreviousBufferIndex();
        m_renderBuffers[prev].m_colorBuffer->bind( shaderType, bindSlot );
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    I32 IRenderTexture::_PreviousBufferIndex()
    {
        I32 index = (m_bufferIndex - 1);
        if (index < 0)
            index += (I32)m_renderBuffers.size();
        return index;
    }

}