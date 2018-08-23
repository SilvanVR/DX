#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#include "../enums.hpp"
#include "../structs.hpp"
#include "Vulkan.hpp"

namespace Graphics { namespace Vulkan { namespace Utility {

    //----------------------------------------------------------------------
    VkCompareOp TranslateComparisonFunc( ComparisonFunc comparison );

    //----------------------------------------------------------------------
    VkBlendFactor TranslateBlend( Blend blend );

    //----------------------------------------------------------------------
    VkBlendOp TranslateBlendOP( BlendOP blendOp );

    //----------------------------------------------------------------------
    std::pair<VkFilter, VkSamplerMipmapMode> TranslateFilter( TextureFilter filter );

    //----------------------------------------------------------------------
    VkSamplerAddressMode TranslateClampMode( TextureAddressMode clampMode );

    //----------------------------------------------------------------------
    VkFormat TranslateTextureFormat( TextureFormat format );

    //----------------------------------------------------------------------
    VkSampleCountFlagBits TranslateSampleCount( SamplingDescription samplingDesc );

    //----------------------------------------------------------------------
    VkFormat TranslateDepthFormat( DepthFormat depth );
    //VkFormat TranslateDepthFormatSRV( DepthFormat depth );

} } }
