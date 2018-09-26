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


    //----------------------------------------------------------------------
    VkCompareOp TranslateComparisonFunc( ComparisonFunc comparison )
    {
        switch (comparison)
        {
        case ComparisonFunc::Never:         return VK_COMPARE_OP_NEVER;
        case ComparisonFunc::Less:          return VK_COMPARE_OP_LESS;
        case ComparisonFunc::Equal:         return VK_COMPARE_OP_EQUAL;
        case ComparisonFunc::LessEqual:     return VK_COMPARE_OP_LESS_OR_EQUAL;
        case ComparisonFunc::Greater:       return VK_COMPARE_OP_GREATER;
        case ComparisonFunc::NotEqual:      return VK_COMPARE_OP_NOT_EQUAL;
        case ComparisonFunc::GreaterEqual:  return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case ComparisonFunc::Always:        return VK_COMPARE_OP_ALWAYS;
        }
        ASSERT( false );
        return VK_COMPARE_OP_ALWAYS;
    }

    //----------------------------------------------------------------------
    VkBlendFactor TranslateBlend( Blend blend )
    {
        switch (blend)
        {
        case Blend::Zero:           return VK_BLEND_FACTOR_ZERO;
        case Blend::One:            return VK_BLEND_FACTOR_ONE;
        case Blend::SrcColor:       return VK_BLEND_FACTOR_SRC_COLOR;
        case Blend::InvSrcColor:    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case Blend::SrcAlpha:       return VK_BLEND_FACTOR_SRC_ALPHA;
        case Blend::InvSrcAlpha:    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case Blend::DestAlpha:      return VK_BLEND_FACTOR_DST_ALPHA;
        case Blend::InvDestAlpha:   return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case Blend::DestColor:      return VK_BLEND_FACTOR_DST_COLOR;
        case Blend::InvDestColor:   return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case Blend::SrcAlphaSat:    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case Blend::BlendFactor:   
        case Blend::InvBlendFactor: ASSERT(false && "Not supported"); break;
        case Blend::Src1Color:      return VK_BLEND_FACTOR_SRC1_COLOR;
        case Blend::InvSrc1Color:   return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case Blend::Src1Alpha:      return VK_BLEND_FACTOR_SRC1_ALPHA;
        case Blend::InvSrc1Alpha:   return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }
        ASSERT( false );
        return VK_BLEND_FACTOR_ONE;
    }

    //----------------------------------------------------------------------
    VkBlendOp TranslateBlendOP( BlendOP blendOp )
    {
        switch (blendOp)
        {
        case BlendOP::Add:          return VK_BLEND_OP_ADD;
        case BlendOP::Max:          return VK_BLEND_OP_MAX;
        case BlendOP::Min:          return VK_BLEND_OP_MIN;
        case BlendOP::Subtract:     return VK_BLEND_OP_SUBTRACT;
        case BlendOP::RevSubtract:  return VK_BLEND_OP_REVERSE_SUBTRACT;
        }

        ASSERT( false );
        return VK_BLEND_OP_ADD;
    }

    //----------------------------------------------------------------------
    std::pair<VkFilter, VkSamplerMipmapMode> TranslateFilter( TextureFilter filter )
    {
        switch (filter)
        {
        case TextureFilter::Point:      return { VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST }; break;
        case TextureFilter::Bilinear:   return { VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST }; break;
        case TextureFilter::Trilinear:  return { VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR }; break;
        }

        ASSERT( false );
        return { VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
    }

    //----------------------------------------------------------------------
    VkSamplerAddressMode TranslateClampMode( TextureAddressMode clampMode )
    {
        switch (clampMode)
        {
        case TextureAddressMode::Clamp:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
        case TextureAddressMode::Mirror:      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; break;
        case TextureAddressMode::MirrorOnce:  return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;  break;
        case TextureAddressMode::Repeat:      return VK_SAMPLER_ADDRESS_MODE_REPEAT; break;
        }

        ASSERT( false );
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    }

    //----------------------------------------------------------------------
    VkFormat TranslateTextureFormat( TextureFormat format )
    {
        switch (format)
        {
        case TextureFormat::Alpha8:         return VK_FORMAT_UNDEFINED; break;
        case TextureFormat::ARGB4444:       return VK_FORMAT_B4G4R4A4_UNORM_PACK16; break;
        case TextureFormat::RGBA32:         return VK_FORMAT_R8G8B8A8_UNORM; break;
        case TextureFormat::RGB565:         return VK_FORMAT_B5G6R5_UNORM_PACK16; break;
        case TextureFormat::R16:            return VK_FORMAT_R16_UNORM; break;
        case TextureFormat::BGRA32:         return VK_FORMAT_B8G8R8A8_UNORM; break;
        case TextureFormat::RHalf:          return VK_FORMAT_R16_SFLOAT; break;
        case TextureFormat::RGHalf:         return VK_FORMAT_R16G16_SFLOAT; break;
        case TextureFormat::RGBAHalf:       return VK_FORMAT_R16G16B16A16_SFLOAT; break;
        case TextureFormat::RFloat:         return VK_FORMAT_R32_SFLOAT; break;
        case TextureFormat::RGFloat:        return VK_FORMAT_R32G32_SFLOAT; break;
        case TextureFormat::RGBAFloat:      return VK_FORMAT_R32G32B32A32_SFLOAT; break;
        case TextureFormat::YUY2:           return VK_FORMAT_G8B8G8R8_422_UNORM; break;
        case TextureFormat::RGB9e5Float:    return VK_FORMAT_UNDEFINED; break;
        case TextureFormat::BC4:            return VK_FORMAT_BC4_UNORM_BLOCK; break;
        case TextureFormat::BC5:            return VK_FORMAT_BC5_UNORM_BLOCK; break;
        case TextureFormat::BC6H:           return VK_FORMAT_BC6H_UFLOAT_BLOCK; break;
        case TextureFormat::BC7:            return VK_FORMAT_BC7_UNORM_BLOCK; break;
        case TextureFormat::RG16:           return VK_FORMAT_R8G8_UINT; break;
        case TextureFormat::R8:             return VK_FORMAT_R8_UNORM; break;
        }

        ASSERT( false && "Oops! Texture format was unknown, which should never happen." );
        return VK_FORMAT_UNDEFINED;
    }

    //----------------------------------------------------------------------
    VkSampleCountFlagBits TranslateSampleCount( MSAASamples samples )
    {
        switch (samples)
        {
        case MSAASamples::One:  return VK_SAMPLE_COUNT_1_BIT;
        case MSAASamples::Two:  return VK_SAMPLE_COUNT_2_BIT;
        case MSAASamples::Four:  return VK_SAMPLE_COUNT_4_BIT;
        case MSAASamples::Eight: return VK_SAMPLE_COUNT_8_BIT;
        }
        ASSERT( false );
        return VK_SAMPLE_COUNT_1_BIT;
    }

    //----------------------------------------------------------------------
    VkFormat TranslateDepthFormat( TextureFormat depth )
    {
        switch (depth)
        {
        case TextureFormat::D16:      return VK_FORMAT_D16_UNORM; break;
        case TextureFormat::D24S8:    return VK_FORMAT_D24_UNORM_S8_UINT; break;
        case TextureFormat::D32:      return VK_FORMAT_D32_SFLOAT; break;
        }
        ASSERT( false && "Oops! Depth format parameter are invalid." );
        return VK_FORMAT_UNDEFINED;
    }

    //----------------------------------------------------------------------
    VkShaderStageFlagBits TranslateShaderStage( ShaderType shaderType )
    {
        switch (shaderType)
        {
        case ShaderType::Vertex:    return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::Fragment:  return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderType::Geometry:  return VK_SHADER_STAGE_GEOMETRY_BIT;
        }
        return {};
    }

} } }
