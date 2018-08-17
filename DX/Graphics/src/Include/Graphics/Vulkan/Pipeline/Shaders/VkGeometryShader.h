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

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void compileFromFile(const OS::Path& path, CString entryPoint) override;
        void compileFromSource(const String& shaderSource, CString entryPoint) override;

    private:
        void _CreateShader(const ShaderBlob& shaderBlob);

        NULL_COPY_AND_ASSIGN(GeometryShader)
    };


} } // End namespaces