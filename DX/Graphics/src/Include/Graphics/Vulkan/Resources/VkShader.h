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
    class ShaderModule;

    //**********************************************************************
    class Shader : public IShader
    {
    public:
        Shader();
        ~Shader();

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void                compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) override;
        void                compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) override;
        void                compileVertexShaderFromSource(const String& src, CString entryPoint) override;
        void                compileFragmentShaderFromSource(const String& src, CString entryPoint) override;
        void                compileGeometryShaderFromSource(const String& src, CString entryPoint) override;
        const VertexLayout& getVertexLayout() const override;
        bool                hasFragmentShader()     const override { return m_pFragmentShader != nullptr; }
        bool                hasGeometryShader()     const override { return m_pGeometryShader != nullptr; }
        bool                hasTessellationShader() const override { return false; }
        void                setRasterizationState(const RasterizationState& rzState) override;
        void                setDepthStencilState(const DepthStencilState& dsState) override;
        void                setBlendState(const BlendState& bState) override;
        void                createPipeline() override;

    private:
        // Shader modules
        std::unique_ptr<ShaderModule> m_pVertexShader = nullptr;
        std::unique_ptr<ShaderModule> m_pFragmentShader  = nullptr;
        std::unique_ptr<ShaderModule> m_pGeometryShader = nullptr;

        // Pipeline state
        VezPipeline                     m_pipeline          = VK_NULL_HANDLE;
        VezVertexInputFormat            m_vertexInputFormat = VK_NULL_HANDLE;
        VezColorBlendAttachmentState    m_blendState{};
        VezRasterizationState           m_rzState{};
        VezDepthStencilState            m_depthStencilState{};

        // Custom api independant data structures
        VertexLayout m_vertexLayout;

        // Contains the data in a contiguous block of memory. Will be empty if not used for a shader.
        //std::unique_ptr<MappedConstantBuffer> m_shaderDataVS = nullptr;
        //std::unique_ptr<MappedConstantBuffer> m_shaderDataPS = nullptr;
        //std::unique_ptr<MappedConstantBuffer> m_shaderDataGS = nullptr;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void _SetInt(StringID name, I32 val)                            override { _UpdateConstantBuffer(name, &val); }
        void _SetFloat(StringID name, F32 val)                          override { _UpdateConstantBuffer(name, &val); }
        void _SetVec4(StringID name, const Math::Vec4& vec)             override { _UpdateConstantBuffer(name, &vec); }
        void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) override { _UpdateConstantBuffer(name, &matrix); }
        void bind() override;
        void unbind() override;

        //----------------------------------------------------------------------
        void _CreatePipeline();
        void _PipelineResourceReflection(VezPipeline pipeline);
        void _CreateVertexLayout(const ArrayList<VezPipelineResource>& resources);
        void _CreateShaderResources(const ArrayList<VezPipelineResource>& resources);
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer(StringID name, const void* pData);
        void _CreateVSConstantBuffer();
        void _CreatePSConstantBuffer();
        void _CreateGSConstantBuffer();

        NULL_COPY_AND_ASSIGN(Shader)
    };

} } // End namespaces