#pragma once
/**********************************************************************
    class: GeometryShader

    author: S. Hau
    date: August 18, 2018
**********************************************************************/

#include "VkShaderBase.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class GeometryShader : public ShaderBase
    {
    public:
        GeometryShader() : ShaderBase(ShaderType::Geometry) {}
        ~GeometryShader() = default;

    private:
        NULL_COPY_AND_ASSIGN(GeometryShader)
    };


} } // End namespaces