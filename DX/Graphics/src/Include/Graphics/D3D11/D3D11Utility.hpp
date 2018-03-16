#pragma once
/**********************************************************************
    class: None (D3D11Utility.hpp)

    author: S. Hau
    date: March 16: 2018

    Contains utility functions for DirectX 11.
**********************************************************************/

#include "../enums.hpp"
#include "D3D11.hpp"

namespace Graphics { namespace D3D11 { namespace Utility {

    //----------------------------------------------------------------------
    D3D11_COMPARISON_FUNC TranslateComparisonFunc( ComparisonFunc comparison )
    {
        switch (comparison)
        {
        case ComparisonFunc::NEVER:         return D3D11_COMPARISON_NEVER;
        case ComparisonFunc::LESS:          return D3D11_COMPARISON_LESS;
        case ComparisonFunc::EQUAL:         return D3D11_COMPARISON_EQUAL;
        case ComparisonFunc::LESS_EQUAL:    return D3D11_COMPARISON_LESS_EQUAL;
        case ComparisonFunc::GREATER:       return D3D11_COMPARISON_GREATER;
        case ComparisonFunc::NOT_EQUAL:     return D3D11_COMPARISON_NOT_EQUAL;
        case ComparisonFunc::GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
        case ComparisonFunc::ALWAYS:        return D3D11_COMPARISON_ALWAYS;
        }
        ASSERT( false );
        return D3D11_COMPARISON_ALWAYS;
    }

    //----------------------------------------------------------------------
    D3D11_BLEND TranslateBlend( Blend blend )
    {
        switch (blend)
        {
        case Blend::ZERO:               return D3D11_BLEND_ZERO;
        case Blend::ONE:                return D3D11_BLEND_ONE;
        case Blend::SRC_COLOR:          return D3D11_BLEND_SRC_COLOR;
        case Blend::INV_SRC_COLOR:      return D3D11_BLEND_INV_SRC_COLOR;
        case Blend::SRC_ALPHA:          return D3D11_BLEND_SRC_ALPHA;
        case Blend::INV_SRC_ALPHA:      return D3D11_BLEND_INV_SRC_ALPHA;
        case Blend::DEST_ALPHA:         return D3D11_BLEND_DEST_ALPHA;
        case Blend::INV_DEST_ALPHA:     return D3D11_BLEND_INV_DEST_ALPHA;
        case Blend::DEST_COLOR:         return D3D11_BLEND_DEST_COLOR;
        case Blend::INV_DEST_COLOR:     return D3D11_BLEND_INV_DEST_COLOR;
        case Blend::SRC_ALPHA_SAT:      return D3D11_BLEND_SRC_ALPHA_SAT;
        case Blend::BLEND_FACTOR:       return D3D11_BLEND_BLEND_FACTOR;
        case Blend::INV_BLEND_FACTOR:   return D3D11_BLEND_INV_BLEND_FACTOR;
        case Blend::SRC1_COLOR:         return D3D11_BLEND_SRC1_COLOR;
        case Blend::INV_SRC1_COLOR:     return D3D11_BLEND_INV_SRC1_COLOR;
        case Blend::SRC1_ALPHA:         return D3D11_BLEND_SRC1_ALPHA;
        case Blend::INV_SRC1_ALPHA:     return D3D11_BLEND_INV_SRC1_ALPHA;
        }

        ASSERT( false );
        return D3D11_BLEND_ONE;
    }

    //----------------------------------------------------------------------
    D3D11_BLEND_OP TranslateBlendOP( BlendOP blendOp )
    {
        switch (blendOp)
        {
        case BlendOP::ADD:          return D3D11_BLEND_OP_ADD;
        case BlendOP::MAX:          return D3D11_BLEND_OP_MAX;
        case BlendOP::MIN:          return D3D11_BLEND_OP_MIN;
        case BlendOP::SUBTRACT:     return D3D11_BLEND_OP_SUBTRACT;
        case BlendOP::REV_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
        }

        ASSERT( false );
        return D3D11_BLEND_OP_ADD;
    }

} } }
