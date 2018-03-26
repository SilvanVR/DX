#include "D3D11IBuffer.h"
/**********************************************************************
    class: IBuffer (D3D11IBuffer.cpp)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    IBuffer::IBuffer( UINT bindFlags, BufferUsage usage, U32 size, const void* pData ) 
        : m_usage( usage ), m_size( size )
    {
        D3D11_BUFFER_DESC bd = {};
        bd.ByteWidth    = size;
        bd.BindFlags    = bindFlags;

        D3D11_USAGE d3d11Usage;
        switch (m_usage)
        {
        case BufferUsage::Immutable:    d3d11Usage = D3D11_USAGE_IMMUTABLE; break;
        case BufferUsage::LongLived:    d3d11Usage = D3D11_USAGE_DEFAULT;   break;
        case BufferUsage::Frequently:
        {
            d3d11Usage = D3D11_USAGE_DYNAMIC;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            break;
        }
        default: ERROR_RENDERING( "IBuffer(): Unknown usage format during buffer creation." );
        }
        bd.Usage = d3d11Usage;

        if (pData != nullptr)
        {
            D3D11_SUBRESOURCE_DATA initData = {};
            initData.pSysMem = pData;
            HR( g_pDevice->CreateBuffer( &bd, &initData, &m_pBuffer ) );
        }
        else
        {
            HR( g_pDevice->CreateBuffer( &bd, NULL, &m_pBuffer ) );
        }
    }

    //----------------------------------------------------------------------
    IBuffer::~IBuffer()
    {
        SAFE_RELEASE( m_pBuffer );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void IBuffer::update( const void* pData, Size sizeInBytes )
    {
        ASSERT( not isImmutable() );

        switch (m_usage)
        {
        case BufferUsage::LongLived:
        {
            g_pImmediateContext->UpdateSubresource( m_pBuffer, 0, NULL, pData, 0, 0 );
            break;
        }
        case BufferUsage::Frequently:
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};
            g_pImmediateContext->Map( m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
            memcpy( mappedResource.pData, pData, sizeInBytes );
            g_pImmediateContext->Unmap( m_pBuffer, 0 );
            break;
        }
        default:
            ASSERT(false);
        }
    }


} } // End namespaces