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
    void RenderTexture::bindForRendering()
    {
        // Bind next buffer
        m_bufferIndex = (m_bufferIndex + 1) % m_renderBuffers.size();

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

    //----------------------------------------------------------------------
    //void RenderTexture::bind( ShaderType shaderType, U32 slot )
    //{
    //    // Bind previous rendered buffer
    //    I32 index = _PreviousBufferIndex();
    //
    //    auto colorBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[index].m_colorBuffer.get() );
    //    colorBuffer->bind( shaderType, slot );
    //}

    //----------------------------------------------------------------------
    //ID3D11Texture2D* RenderTexture::getD3D11Texture() 
    //{ 
    //    auto colorBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_colorBuffer.get() );
    //    return colorBuffer->m_pTexture;
    //}

} } // End namespaces