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
        VertexShader() = default;
        ~VertexShader();

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void bind() override;
        bool compile(const OS::Path& path, CString entryPoint) override;
        bool compile(const String& shaderSource, CString entryPoint) override;
        bool recompile() override;

        //----------------------------------------------------------------------
        const VertexLayout& getVertexLayout() const { return m_vertexLayout; }

    private:
        ID3D11VertexShader* m_pVertexShader = nullptr;
        ID3D11InputLayout*  m_pInputLayout  = nullptr;

        VertexLayout        m_vertexLayout;

        //----------------------------------------------------------------------
        void _CreateInputLayout(ID3DBlob* pShaderBlob);
        void _AddToVertexLayout(const String& semanticName);


        //----------------------------------------------------------------------
        VertexShader(const VertexShader& other)               = delete;
        VertexShader& operator = (const VertexShader& other)  = delete;
        VertexShader(VertexShader&& other)                    = delete;
        VertexShader& operator = (VertexShader&& other)       = delete;
    };

} } // End namespaces