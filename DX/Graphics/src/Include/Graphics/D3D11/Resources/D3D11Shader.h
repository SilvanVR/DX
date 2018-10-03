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
        ~Shader() = default;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void                compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) override;
        void                compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) override;
        void                compileVertexShaderFromSource(const String& src, CString entryPoint) override;
        void                compileFragmentShaderFromSource(const String& src, CString entryPoint) override;
        void                compileGeometryShaderFromSource(const String& src, CString entryPoint) override;
        const VertexLayout& getVertexLayout() const override;
        bool                hasFragmentShader()     const override { return m_pPixelShader != nullptr; }
        bool                hasGeometryShader()     const override { return m_pGeometryShader != nullptr; }
        bool                hasTessellationShader() const override { return false; }
        void                createPipeline() override;

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

        ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
        ComPtr<ID3D11RasterizerState>   m_pRSState;
        ComPtr<ID3D11BlendState>        m_pBlendState;

        // Contains the data in a contiguous block of memory. Will be empty if not used for a shader.
        std::unique_ptr<MappedConstantBuffer> m_shaderDataVS = nullptr;
        std::unique_ptr<MappedConstantBuffer> m_shaderDataPS = nullptr;
        std::unique_ptr<MappedConstantBuffer> m_shaderDataGS = nullptr;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void bind() override;
        void unbind() override;

        //----------------------------------------------------------------------
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer(StringID name, const void* pData);

        NULL_COPY_AND_ASSIGN(Shader)
    };

} } // End namespaces