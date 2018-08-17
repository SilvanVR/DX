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

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void compileFromFile(const OS::Path& path, CString entryPoint) override;
        void compileFromSource(const String& shaderSource, CString entryPoint) override;

    private:
        void _CreateShader(const ShaderBlob& shaderBlob);

        NULL_COPY_AND_ASSIGN(FragmentShader)
    };


} } // End namespaces