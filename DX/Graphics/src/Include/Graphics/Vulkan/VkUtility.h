#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#include "../enums.hpp"
#include "Vulkan.hpp"

namespace Graphics { namespace Vulkan { namespace Utility {

    ////----------------------------------------------------------------------
    //D3D11_COMPARISON_FUNC TranslateComparisonFunc( ComparisonFunc comparison );

    ////----------------------------------------------------------------------
    //D3D11_BLEND TranslateBlend( Blend blend );

    ////----------------------------------------------------------------------
    //D3D11_BLEND_OP TranslateBlendOP( BlendOP blendOp );

    ////----------------------------------------------------------------------
    //D3D11_FILTER TranslateFilter( TextureFilter filter );

    ////----------------------------------------------------------------------
    //D3D11_TEXTURE_ADDRESS_MODE TranslateClampMode( TextureAddressMode clampMode );

    ////----------------------------------------------------------------------
    //DXGI_FORMAT TranslateTextureFormat( TextureFormat format );

    ////----------------------------------------------------------------------
    //DXGI_FORMAT TranslateDepthFormat( DepthFormat depth );
    //DXGI_FORMAT TranslateDepthFormatSRV( DepthFormat depth );

    ////----------------------------------------------------------------------
    //bool MSAASamplesSupported( DXGI_FORMAT format, U8 numSamples );

} } }
