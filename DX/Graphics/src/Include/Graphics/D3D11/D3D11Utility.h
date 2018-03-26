#pragma once
/**********************************************************************
    class: None (D3D11Utility.h)

    author: S. Hau
    date: March 16 2018

    Contains utility functions for DirectX 11.
**********************************************************************/

#include "../enums.hpp"
#include "D3D11.hpp"

namespace Graphics { namespace D3D11 { namespace Utility {

    //----------------------------------------------------------------------
    D3D11_COMPARISON_FUNC TranslateComparisonFunc( ComparisonFunc comparison );

    //----------------------------------------------------------------------
    D3D11_BLEND TranslateBlend( Blend blend );

    //----------------------------------------------------------------------
    D3D11_BLEND_OP TranslateBlendOP( BlendOP blendOp );

    //----------------------------------------------------------------------
    D3D11_FILTER TranslateFilter( TextureFilter filter );

    //----------------------------------------------------------------------
    D3D11_TEXTURE_ADDRESS_MODE TranslateClampMode( TextureAddressMode clampMode );

    //----------------------------------------------------------------------
    DXGI_FORMAT TranslateTextureFormat( TextureFormat format );

} } }
