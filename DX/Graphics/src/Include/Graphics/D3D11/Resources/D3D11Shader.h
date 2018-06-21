#pragma once
/**********************************************************************
    class: Shader (Shader.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_shader.h"
#include "../D3D11.hpp"
#include "D3D11/Pipeline/Buffers/D3D11MappedConstantBuffer.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    class VertexShader;
    class PixelShader;
    class GeometryShader;

    //**********************************************************************
    class Shader : public IShader
    {
    public:
        Shader();
        ~Shader();

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        bool                                    compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) override;
        bool                                    compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) override;
        bool                                    compileVertexShaderFromSource(const String& src, CString entryPoint) override;
        bool                                    compileFragmentShaderFromSource(const String& src, CString entryPoint) override;
        bool                                    compileGeometryShaderFromSource(const String& src, CString entryPoint) override;
        ArrayList<OS::Path>                     recompile();
        bool                                    isUpToDate() override;
        ArrayList<OS::Path>                     getShaderPaths() const override;
        const VertexLayout&                     getVertexLayout() const override;
        const ShaderResourceDeclaration*        getShaderResource(StringID name) const override;
        const ShaderUniformBufferDeclaration*   getVSUniformMaterialBuffer() const override;
        const ShaderUniformBufferDeclaration*   getFSUniformMaterialBuffer() const override;
        const ShaderUniformBufferDeclaration*   getGSUniformMaterialBuffer() const override;
        const ShaderUniformBufferDeclaration*   getVSUniformShaderBuffer() const override;
        const ShaderUniformBufferDeclaration*   getFSUniformShaderBuffer() const override;
        const ShaderUniformBufferDeclaration*   getGSUniformShaderBuffer() const override;
        bool                                    hasFragmentShader()     const override { return m_pPixelShader != nullptr; }
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
        const VertexShader* getVertexShader() const { return m_pVertexShader.get(); }
        const PixelShader*  getPixelShader() const { return m_pPixelShader.get(); }

    private:
        std::unique_ptr<VertexShader>   m_pVertexShader = nullptr;
        std::unique_ptr<PixelShader>    m_pPixelShader  = nullptr;
        std::unique_ptr<GeometryShader> m_pGeometryShader = nullptr;

        ID3D11DepthStencilState*        m_pDepthStencilState;
        ID3D11RasterizerState*          m_pRSState;
        ID3D11BlendState*               m_pBlendState;

        // Contains the data in a contiguous block of memory. Will be empty if not used for a shader.
        MappedConstantBuffer* m_shaderDataVS = nullptr;
        MappedConstantBuffer* m_shaderDataPS = nullptr;
        MappedConstantBuffer* m_shaderDataGS = nullptr;


        bool m_scissorEnabled = false;

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

        NULL_COPY_AND_ASSIGN(Shader)
    };

} } // End namespaces