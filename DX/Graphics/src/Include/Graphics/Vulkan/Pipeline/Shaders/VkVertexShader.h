#pragma once
/**********************************************************************
    class: VertexShader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "VkShaderBase.h"
#include "vertex_layout.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class VertexShader : public ShaderBase
    {
    public:
        VertexShader() : ShaderBase(ShaderType::Vertex) {}
        ~VertexShader() = default;

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void compileFromFile(const OS::Path& path, CString entryPoint) override;
        void compileFromSource(const String& shaderSource, CString entryPoint) override;

        //----------------------------------------------------------------------
        const VertexLayout&     getVertexLayout()   const { return m_vertexLayout; }
        const VkShaderModule&   getVkShaderModule() const { return m_shaderModule; }

    private:
        VkShaderModule  m_shaderModule;
        VertexLayout    m_vertexLayout;

        //----------------------------------------------------------------------
        //void _CreateInputLayout(const void* pShaderByteCode, Size sizeInBytes);
        //void _AddToVertexLayout(const String& semanticName, U32 semanticIndex, U32 sizeInBytes, bool instanced);
        void _CreateShader(const ShaderBlob& shaderBlob);

        NULL_COPY_AND_ASSIGN(VertexShader)
    };

} } // End namespaces