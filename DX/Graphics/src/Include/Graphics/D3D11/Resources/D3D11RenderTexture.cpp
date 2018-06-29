#include "D3D11RenderTexture.h"
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "D3D11RenderBuffer.h"

namespace Graphics { namespace D3D11 {


    //**********************************************************************
    // PUBLIC
    //**********************************************************************

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
        auto colorBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_colorBuffer.get() );
        auto depthBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_depthBuffer.get() );

        if (colorBuffer && depthBuffer)
        {
            colorBuffer->_ClearResolvedFlag();
            depthBuffer->_ClearResolvedFlag();
            g_pImmediateContext->OMSetRenderTargets( 1, &colorBuffer->m_pRenderTargetView, depthBuffer->m_pDepthStencilView );
        }
        else if (depthBuffer)
        {
            depthBuffer->_ClearResolvedFlag();
            g_pImmediateContext->OMSetRenderTargets( 0, nullptr, depthBuffer->m_pDepthStencilView );
        }
        else
        {
            colorBuffer->_ClearResolvedFlag();
            g_pImmediateContext->OMSetRenderTargets( 1, &colorBuffer->m_pRenderTargetView, nullptr );
        }
    }

} } // End namespaces