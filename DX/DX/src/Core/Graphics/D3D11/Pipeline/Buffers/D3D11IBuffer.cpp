#include "D3D11IBuffer.h"
/**********************************************************************
    class: IBuffer (D3D11IBuffer.cpp)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

namespace Core { namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    IBuffer::IBuffer( UINT bindFlags, D3D11_USAGE usage, U32 size, const void* pData ) : m_size( size )
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage        = usage;
        bd.ByteWidth    = size;
        bd.BindFlags    = bindFlags;

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

} } } // End namespaces