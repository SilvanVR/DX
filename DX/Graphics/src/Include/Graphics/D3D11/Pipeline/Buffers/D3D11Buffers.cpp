#include "D3D11Buffers.h"
/**********************************************************************
    class: VertexBuffer, IndexBuffer, ConstantBuffer (D3D11Buffers.cpp)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    VertexBuffer::VertexBuffer( U32 size, const void* pData )
        : IBuffer( D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, size, pData )
    {
    }

    //----------------------------------------------------------------------
    void VertexBuffer::bind( U32 slot, U32 stride, U32 offset )
    {
        g_pImmediateContext->IASetVertexBuffers( slot, 1, &m_pBuffer, &stride, &offset );
    }

    //**********************************************************************
    //----------------------------------------------------------------------
    IndexBuffer::IndexBuffer( U32 size, const void* pData )
        : IBuffer( D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, size, pData )
    {
    }

    //----------------------------------------------------------------------
    void IndexBuffer::bind( DXGI_FORMAT format, U32 offset )
    {
        g_pImmediateContext->IASetIndexBuffer( m_pBuffer, format, offset );
    }

    //**********************************************************************
    //----------------------------------------------------------------------
    ConstantBuffer::ConstantBuffer( U32 size )
        : IBuffer( D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT, size )
    {
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::bind( U32 slot )
    {
        g_pImmediateContext->VSSetConstantBuffers( slot, 1, &m_pBuffer );
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::updateSubresource( const void* pData )
    {
        g_pImmediateContext->UpdateSubresource( m_pBuffer, 0, NULL, pData, 0, 0 );
    }

} } // End namespaces