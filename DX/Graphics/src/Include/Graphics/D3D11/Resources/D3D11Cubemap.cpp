#include "D3D11Cubemap.h"
/**********************************************************************
    class: Cubemap (D3D11Cubemap.cpp)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Cubemap::~Cubemap()
    {

    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Cubemap::bind( U32 slot )
    {
        if (not m_gpuUpToDate)
        {
            _PushToGPU();
            m_gpuUpToDate = true;
        }

        if (m_generateMips)
        {
            g_pImmediateContext->GenerateMips( m_pTextureView );
            m_generateMips = false;
        }

        g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );
        g_pImmediateContext->PSSetShaderResources( slot, 1, &m_pTextureView );
    }

    //----------------------------------------------------------------------
    void Cubemap::create( I32 size, TextureFormat format, bool generateMips )
    {
        ITexture::_Init( size, size, format );
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void Cubemap::setPixel( CubemapFace face, I32 x, I32 y, Color color )
    {

    }

    //----------------------------------------------------------------------
    void Cubemap::setPixels( CubemapFace face, const void* pPixels )
    {

    }

    //----------------------------------------------------------------------
    void Cubemap::apply( bool updateMips )
    {

    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Cubemap::_PushToGPU()
    {

    }

} } // End namespaces