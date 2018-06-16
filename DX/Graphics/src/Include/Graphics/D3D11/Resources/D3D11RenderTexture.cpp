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
    void RenderTexture::bind( ShaderType shaderType, U32 slot )
    {
        // Bind previous rendered buffer
        I32 index = _PreviousBufferIndex();

        auto colorBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[index].m_colorBuffer.get() );
        colorBuffer->bind( shaderType, slot );
    }

    //----------------------------------------------------------------------
    void RenderTexture::bindForRendering()
    {
        // Bind next buffer
        m_bufferIndex = (m_bufferIndex + 1) % m_renderBuffers.size();

        auto colorBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_colorBuffer.get() );
        auto depthBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_depthBuffer.get() );
        colorBuffer->_ClearResolvedFlag();

        if (depthBuffer)
        {
            depthBuffer->_ClearResolvedFlag();
            g_pImmediateContext->OMSetRenderTargets( 1, &colorBuffer->m_pRenderTargetView, depthBuffer->m_pDepthStencilView );
        }
        else
        {
            g_pImmediateContext->OMSetRenderTargets( 1, &colorBuffer->m_pRenderTargetView, nullptr );
        }
    }

    //----------------------------------------------------------------------
    ID3D11Texture2D* RenderTexture::getD3D11Texture() 
    { 
        auto colorBuffer = reinterpret_cast<D3D11::RenderBuffer*>( m_renderBuffers[m_bufferIndex].m_colorBuffer.get() );
        return colorBuffer->m_pRenderBuffer;
    }

} } // End namespaces