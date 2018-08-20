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
    void RenderTexture::bindForRendering( U64 frameIndex )
    {
        // This is a little hack in order to advance the buffer index only once, because it can be bound several times per frame
        if (m_curFrameIndex != frameIndex)
        {
            m_bufferIndex = (m_bufferIndex + 1) % m_renderBuffers.size();
            m_curFrameIndex = frameIndex;
        }

        // Bind buffer
        auto colorBuffer = reinterpret_cast<Vulkan::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_colorBuffer.get() );
        auto depthBuffer = reinterpret_cast<Vulkan::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_depthBuffer.get() );

        if (colorBuffer && depthBuffer)
        {
            colorBuffer->_ClearResolvedFlag();
            depthBuffer->_ClearResolvedFlag();
            //g_pImmediateContext->OMSetRenderTargets( 1, &colorBuffer->m_pRenderTargetView, depthBuffer->m_pDepthStencilView );
        }
        else if (depthBuffer)
        {
            depthBuffer->_ClearResolvedFlag();
            //g_pImmediateContext->OMSetRenderTargets( 0, nullptr, depthBuffer->m_pDepthStencilView );
        }
        else
        {
            colorBuffer->_ClearResolvedFlag();
            //g_pImmediateContext->OMSetRenderTargets( 1, &colorBuffer->m_pRenderTargetView, nullptr );
        }
    }

} } // End namespaces