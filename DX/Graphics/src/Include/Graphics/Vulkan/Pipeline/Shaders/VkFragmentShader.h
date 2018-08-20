#pragma once
/**********************************************************************
    class: FragmentShader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "VkShaderBase.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class FragmentShader : public ShaderBase
    {
    public:
        FragmentShader() : ShaderBase(ShaderType::Fragment) {}
        ~FragmentShader() = default;

    private:
        NULL_COPY_AND_ASSIGN(FragmentShader)
    };


} } // End namespaces