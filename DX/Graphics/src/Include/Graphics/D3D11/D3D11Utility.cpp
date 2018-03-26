#include "D3D11Utility.h"
/**********************************************************************
    class: None (D3D11Utility.cpp)

    author: S. Hau
    date: March 26 2018
**********************************************************************/

namespace Graphics { namespace D3D11 { namespace Utility {

    //----------------------------------------------------------------------
    D3D11_COMPARISON_FUNC TranslateComparisonFunc( ComparisonFunc comparison )
    {
        switch (comparison)
        {
        case ComparisonFunc::Never:         return D3D11_COMPARISON_NEVER;
        case ComparisonFunc::Less:          return D3D11_COMPARISON_LESS;
        case ComparisonFunc::Equal:         return D3D11_COMPARISON_EQUAL;
        case ComparisonFunc::LessEqual:     return D3D11_COMPARISON_LESS_EQUAL;
        case ComparisonFunc::Greater:       return D3D11_COMPARISON_GREATER;
        case ComparisonFunc::NotEqual:      return D3D11_COMPARISON_NOT_EQUAL;
        case ComparisonFunc::GreaterEqual:  return D3D11_COMPARISON_GREATER_EQUAL;
        case ComparisonFunc::Always:        return D3D11_COMPARISON_ALWAYS;
        }
        ASSERT( false );
        return D3D11_COMPARISON_ALWAYS;
    }

    //----------------------------------------------------------------------
    D3D11_BLEND TranslateBlend( Blend blend )
    {
        switch (blend)
        {
        case Blend::Zero:           return D3D11_BLEND_ZERO;
        case Blend::One:            return D3D11_BLEND_ONE;
        case Blend::SrcColor:       return D3D11_BLEND_SRC_COLOR;
        case Blend::InvSrcColor:    return D3D11_BLEND_INV_SRC_COLOR;
        case Blend::SrcAlpha:       return D3D11_BLEND_SRC_ALPHA;
        case Blend::InvSrcAlpha:    return D3D11_BLEND_INV_SRC_ALPHA;
        case Blend::DestAlpha:      return D3D11_BLEND_DEST_ALPHA;
        case Blend::InvDestAlpha:   return D3D11_BLEND_INV_DEST_ALPHA;
        case Blend::DestColor:      return D3D11_BLEND_DEST_COLOR;
        case Blend::InvDestColor:   return D3D11_BLEND_INV_DEST_COLOR;
        case Blend::SrcAlphaSat:    return D3D11_BLEND_SRC_ALPHA_SAT;
        case Blend::BlendFactor:    return D3D11_BLEND_BLEND_FACTOR;
        case Blend::InvBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
        case Blend::Src1Color:      return D3D11_BLEND_SRC1_COLOR;
        case Blend::InvSrc1Color:   return D3D11_BLEND_INV_SRC1_COLOR;
        case Blend::Src1Alpha:      return D3D11_BLEND_SRC1_ALPHA;
        case Blend::InvSrc1Alpha:   return D3D11_BLEND_INV_SRC1_ALPHA;
        }

        ASSERT( false );
        return D3D11_BLEND_ONE;
    }

    //----------------------------------------------------------------------
    D3D11_BLEND_OP TranslateBlendOP( BlendOP blendOp )
    {
        switch (blendOp)
        {
        case BlendOP::Add:          return D3D11_BLEND_OP_ADD;
        case BlendOP::Max:          return D3D11_BLEND_OP_MAX;
        case BlendOP::Min:          return D3D11_BLEND_OP_MIN;
        case BlendOP::Subtract:     return D3D11_BLEND_OP_SUBTRACT;
        case BlendOP::RevSubtract:  return D3D11_BLEND_OP_REV_SUBTRACT;
        }

        ASSERT( false );
        return D3D11_BLEND_OP_ADD;
    }

    //----------------------------------------------------------------------
    D3D11_FILTER TranslateFilter( TextureFilter filter )
    {
        switch (filter)
        {
        case TextureFilter::Point:      return D3D11_FILTER_MIN_MAG_MIP_POINT; break;
        case TextureFilter::Bilinear:   return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; break;
        case TextureFilter::Trilinear:  return D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
        }

        ASSERT( false );
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }

    //----------------------------------------------------------------------
    D3D11_TEXTURE_ADDRESS_MODE TranslateClampMode( TextureAddressMode clampMode )
    {
        switch (clampMode)
        {
        case TextureAddressMode::Clamp:       return D3D11_TEXTURE_ADDRESS_CLAMP; break;
        case TextureAddressMode::Mirror:      return D3D11_TEXTURE_ADDRESS_MIRROR; break;
        case TextureAddressMode::MirrorOnce:  return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;  break;
        case TextureAddressMode::Repeat:      return D3D11_TEXTURE_ADDRESS_WRAP; break;
        }

        ASSERT( false );
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    }

} } }
