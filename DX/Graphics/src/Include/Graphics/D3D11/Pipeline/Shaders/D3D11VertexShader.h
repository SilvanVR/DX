#pragma once
/**********************************************************************
    class: VertexShader (D3D11VertexShader.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "D3D11ShaderBase.h"
#include "vertex_layout.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class VertexShader : public ShaderBase
    {
    public:
        VertexShader() : ShaderBase(ShaderType::Vertex) {}
        ~VertexShader() = default;

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void bind() override;
        void unbind() override;
        void compileFromFile(const OS::Path& path, CString entryPoint) override;
        void compileFromSource(const String& shaderSource, CString entryPoint) override;

        //----------------------------------------------------------------------
        const VertexLayout& getVertexLayout() const { return m_vertexLayout; }

    private:
        ComPtr<ID3D11VertexShader> m_pVertexShader = nullptr;
        ComPtr<ID3D11InputLayout>  m_pInputLayout  = nullptr;

        VertexLayout m_vertexLayout;

        //----------------------------------------------------------------------
        void _CreateInputLayout(const void* pShaderByteCode, Size sizeInBytes);
        void _AddToVertexLayout(const String& semanticName, U32 semanticIndex, U32 sizeInBytes, bool instanced);
        void _CreateD3D11VertexShader(const ShaderBlob& shaderBlob);

        NULL_COPY_AND_ASSIGN(VertexShader)
    };

} } // End namespaces