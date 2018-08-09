#include "VkUtility.h"
/**********************************************************************
    class: None

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

String ResultToString(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS: return "SUCCESS";
    case VK_NOT_READY: return "NOT_READY";
    case VK_TIMEOUT: return "TIMEOUT";
    case VK_EVENT_SET: return "EVENT_SET";
    case VK_EVENT_RESET: return "EVENT_RESET";
    case VK_INCOMPLETE: return "INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_SURFACE_LOST_KHR: return "ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR: return "SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT: return "ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV: return "ERROR_INVALID_SHADER_NV";
    default: return std::to_string(result);
    }
}

namespace Graphics { namespace Vulkan { namespace Utility {


    ////----------------------------------------------------------------------
    //D3D11_COMPARISON_FUNC TranslateComparisonFunc( ComparisonFunc comparison )
    //{
    //    switch (comparison)
    //    {
    //    case ComparisonFunc::Never:         return D3D11_COMPARISON_NEVER;
    //    case ComparisonFunc::Less:          return D3D11_COMPARISON_LESS;
    //    case ComparisonFunc::Equal:         return D3D11_COMPARISON_EQUAL;
    //    case ComparisonFunc::LessEqual:     return D3D11_COMPARISON_LESS_EQUAL;
    //    case ComparisonFunc::Greater:       return D3D11_COMPARISON_GREATER;
    //    case ComparisonFunc::NotEqual:      return D3D11_COMPARISON_NOT_EQUAL;
    //    case ComparisonFunc::GreaterEqual:  return D3D11_COMPARISON_GREATER_EQUAL;
    //    case ComparisonFunc::Always:        return D3D11_COMPARISON_ALWAYS;
    //    }
    //    ASSERT( false );
    //    return D3D11_COMPARISON_ALWAYS;
    //}

    ////----------------------------------------------------------------------
    //D3D11_BLEND TranslateBlend( Blend blend )
    //{
    //    switch (blend)
    //    {
    //    case Blend::Zero:           return D3D11_BLEND_ZERO;
    //    case Blend::One:            return D3D11_BLEND_ONE;
    //    case Blend::SrcColor:       return D3D11_BLEND_SRC_COLOR;
    //    case Blend::InvSrcColor:    return D3D11_BLEND_INV_SRC_COLOR;
    //    case Blend::SrcAlpha:       return D3D11_BLEND_SRC_ALPHA;
    //    case Blend::InvSrcAlpha:    return D3D11_BLEND_INV_SRC_ALPHA;
    //    case Blend::DestAlpha:      return D3D11_BLEND_DEST_ALPHA;
    //    case Blend::InvDestAlpha:   return D3D11_BLEND_INV_DEST_ALPHA;
    //    case Blend::DestColor:      return D3D11_BLEND_DEST_COLOR;
    //    case Blend::InvDestColor:   return D3D11_BLEND_INV_DEST_COLOR;
    //    case Blend::SrcAlphaSat:    return D3D11_BLEND_SRC_ALPHA_SAT;
    //    case Blend::BlendFactor:    return D3D11_BLEND_BLEND_FACTOR;
    //    case Blend::InvBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
    //    case Blend::Src1Color:      return D3D11_BLEND_SRC1_COLOR;
    //    case Blend::InvSrc1Color:   return D3D11_BLEND_INV_SRC1_COLOR;
    //    case Blend::Src1Alpha:      return D3D11_BLEND_SRC1_ALPHA;
    //    case Blend::InvSrc1Alpha:   return D3D11_BLEND_INV_SRC1_ALPHA;
    //    }

    //    ASSERT( false );
    //    return D3D11_BLEND_ONE;
    //}

    ////----------------------------------------------------------------------
    //D3D11_BLEND_OP TranslateBlendOP( BlendOP blendOp )
    //{
    //    switch (blendOp)
    //    {
    //    case BlendOP::Add:          return D3D11_BLEND_OP_ADD;
    //    case BlendOP::Max:          return D3D11_BLEND_OP_MAX;
    //    case BlendOP::Min:          return D3D11_BLEND_OP_MIN;
    //    case BlendOP::Subtract:     return D3D11_BLEND_OP_SUBTRACT;
    //    case BlendOP::RevSubtract:  return D3D11_BLEND_OP_REV_SUBTRACT;
    //    }

    //    ASSERT( false );
    //    return D3D11_BLEND_OP_ADD;
    //}

    ////----------------------------------------------------------------------
    //D3D11_FILTER TranslateFilter( TextureFilter filter )
    //{
    //    switch (filter)
    //    {
    //    case TextureFilter::Point:      return D3D11_FILTER_MIN_MAG_MIP_POINT; break;
    //    case TextureFilter::Bilinear:   return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; break;
    //    case TextureFilter::Trilinear:  return D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
    //    }

    //    ASSERT( false );
    //    return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //}

    ////----------------------------------------------------------------------
    //D3D11_TEXTURE_ADDRESS_MODE TranslateClampMode( TextureAddressMode clampMode )
    //{
    //    switch (clampMode)
    //    {
    //    case TextureAddressMode::Clamp:       return D3D11_TEXTURE_ADDRESS_CLAMP; break;
    //    case TextureAddressMode::Mirror:      return D3D11_TEXTURE_ADDRESS_MIRROR; break;
    //    case TextureAddressMode::MirrorOnce:  return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;  break;
    //    case TextureAddressMode::Repeat:      return D3D11_TEXTURE_ADDRESS_WRAP; break;
    //    }

    //    ASSERT( false );
    //    return D3D11_TEXTURE_ADDRESS_CLAMP;
    //}

    ////----------------------------------------------------------------------
    //DXGI_FORMAT TranslateTextureFormat( TextureFormat format )
    //{
    //    switch (format)
    //    {
    //    case TextureFormat::Alpha8:         return DXGI_FORMAT_A8_UNORM; break;
    //    case TextureFormat::ARGB4444:       return DXGI_FORMAT_B4G4R4A4_UNORM; break;
    //    case TextureFormat::RGBA32:         return DXGI_FORMAT_R8G8B8A8_UNORM; break;
    //    case TextureFormat::RGB565:         return DXGI_FORMAT_B5G6R5_UNORM; break;
    //    case TextureFormat::R16:            return DXGI_FORMAT_R16_UNORM; break;
    //    case TextureFormat::BGRA32:         return DXGI_FORMAT_B8G8R8A8_UNORM; break;
    //    case TextureFormat::RHalf:          return DXGI_FORMAT_R16_FLOAT; break;
    //    case TextureFormat::RGHalf:         return DXGI_FORMAT_R16G16_FLOAT; break;
    //    case TextureFormat::RGBAHalf:       return DXGI_FORMAT_R16G16B16A16_FLOAT; break;
    //    case TextureFormat::RFloat:         return DXGI_FORMAT_R32_FLOAT; break;
    //    case TextureFormat::RGFloat:        return DXGI_FORMAT_R32G32_FLOAT; break;
    //    case TextureFormat::RGBAFloat:      return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
    //    case TextureFormat::YUY2:           return DXGI_FORMAT_G8R8_G8B8_UNORM; break;
    //    case TextureFormat::RGB9e5Float:    return DXGI_FORMAT_R9G9B9E5_SHAREDEXP; break;
    //    case TextureFormat::BC4:            return DXGI_FORMAT_BC4_UNORM; break;
    //    case TextureFormat::BC5:            return DXGI_FORMAT_BC5_UNORM; break;
    //    case TextureFormat::BC6H:           return DXGI_FORMAT_BC6H_UF16; /*DXGI_FORMAT_BC6H_SF16*/ break;
    //    case TextureFormat::BC7:            return DXGI_FORMAT_BC7_UNORM; /*DXGI_FORMAT_BC7_UNORM_SRGB*/ break;
    //    case TextureFormat::RG16:           return DXGI_FORMAT_R8G8_UINT; break;
    //    case TextureFormat::R8:             return DXGI_FORMAT_R8_UNORM;  break;
    //    }

    //    ASSERT( false && "Oops! Texture format was unknown, which should never happen." );
    //    return DXGI_FORMAT_A8_UNORM;
    //}

    ////----------------------------------------------------------------------
    //DXGI_FORMAT TranslateDepthFormat( DepthFormat depth )
    //{
    //    switch (depth)
    //    {
    //    case DepthFormat::D16:      return DXGI_FORMAT_D16_UNORM; break;
    //    case DepthFormat::D24S8:    return DXGI_FORMAT_D24_UNORM_S8_UINT; break;
    //    case DepthFormat::D32:      return DXGI_FORMAT_D32_FLOAT; break;
    //    }
    //    ASSERT( false && "Oops! Depth format parameter are invalid." );
    //    return DXGI_FORMAT_D16_UNORM;
    //}

    ////----------------------------------------------------------------------
    //DXGI_FORMAT TranslateDepthFormatSRV( DepthFormat depth )
    //{
    //    switch (depth)
    //    {
    //    case DepthFormat::D16:      return DXGI_FORMAT_R16_TYPELESS; break;
    //    case DepthFormat::D24S8:    return DXGI_FORMAT_R24G8_TYPELESS; break;
    //    case DepthFormat::D32:      return DXGI_FORMAT_R32_TYPELESS; break;
    //    }
    //    ASSERT( false && "Oops! Depth format parameter are invalid." );
    //    return DXGI_FORMAT_D16_UNORM;
    //}

    ////----------------------------------------------------------------------
    //bool MSAASamplesSupported( DXGI_FORMAT format, U8 numSamples )
    //{
    //    if (numSamples > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
    //        return false;

    //    UINT msaaQualityLevels;
    //    HR( g_pDevice->CheckMultisampleQualityLevels( format, numSamples, &msaaQualityLevels ) );
    //    if (msaaQualityLevels == 0)
    //        return false;

    //    return true;
    //}

} } }
