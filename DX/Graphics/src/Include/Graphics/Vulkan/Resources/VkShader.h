#pragma once
/**********************************************************************
    class: Shader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "../../i_shader.h"
#include "Vulkan/Vulkan.hpp"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    class VertexShader;
    class FragmentShader;
    class GeometryShader;

    //**********************************************************************
    class Shader : public IShader
    {
    public:
        Shader();
        ~Shader() = default;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void                                    compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) override;
        void                                    compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) override;
        void                                    compileVertexShaderFromSource(const String& src, CString entryPoint) override;
        void                                    compileFragmentShaderFromSource(const String& src, CString entryPoint) override;
        void                                    compileGeometryShaderFromSource(const String& src, CString entryPoint) override;
        const VertexLayout&                     getVertexLayout() const override;
        const ShaderResourceDeclaration*        getShaderResource(StringID name) const override;
        const ShaderUniformBufferDeclaration*   getVSUniformMaterialBuffer() const override;
        const ShaderUniformBufferDeclaration*   getFSUniformMaterialBuffer() const override;
        const ShaderUniformBufferDeclaration*   getGSUniformMaterialBuffer() const override;
        const ShaderUniformBufferDeclaration*   getVSUniformShaderBuffer() const override;
        const ShaderUniformBufferDeclaration*   getFSUniformShaderBuffer() const override;
        const ShaderUniformBufferDeclaration*   getGSUniformShaderBuffer() const override;
        bool                                    hasFragmentShader()     const override { return m_pFragmentShader != nullptr; }
        bool                                    hasGeometryShader()     const override { return m_pGeometryShader != nullptr; }
        bool                                    hasTessellationShader() const override { return false; }

        void _SetInt(StringID name, I32 val)                            override { _UpdateConstantBuffer(name, &val); }
        void _SetFloat(StringID name, F32 val)                          override { _UpdateConstantBuffer(name, &val); }
        void _SetVec4(StringID name, const Math::Vec4& vec)             override { _UpdateConstantBuffer(name, &vec); }
        void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) override { _UpdateConstantBuffer(name, &matrix); }

        void setRasterizationState(const RasterizationState& rzState) override;
        void setDepthStencilState(const DepthStencilState& dsState) override;
        void setBlendState(const BlendState& bState) override;

        //----------------------------------------------------------------------
        const VertexShader*     getVertexShader() const { return m_pVertexShader.get(); }
        const FragmentShader*   getFragmentShader() const { return m_pFragmentShader.get(); }

    private:
        std::unique_ptr<VertexShader>   m_pVertexShader = nullptr;
        std::unique_ptr<FragmentShader> m_pFragmentShader  = nullptr;
        std::unique_ptr<GeometryShader> m_pGeometryShader = nullptr;
        
        VkPipelineColorBlendAttachmentState     m_blendState{};
        VkPipelineDepthStencilStateCreateInfo   m_depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        VkPipelineRasterizationStateCreateInfo  m_rzState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

        // Contains the data in a contiguous block of memory. Will be empty if not used for a shader.
        //std::unique_ptr<MappedConstantBuffer> m_shaderDataVS = nullptr;
        //std::unique_ptr<MappedConstantBuffer> m_shaderDataPS = nullptr;
        //std::unique_ptr<MappedConstantBuffer> m_shaderDataGS = nullptr;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void bind() override;
        void unbind() override;

        //----------------------------------------------------------------------
        void _CreatePipeline();
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer(StringID name, const void* pData);
        void _CreateVSConstantBuffer();
        void _CreatePSConstantBuffer();
        void _CreateGSConstantBuffer();
        void _CreatePipelineLayout();

        NULL_COPY_AND_ASSIGN(Shader)
    };

} } // End namespaces