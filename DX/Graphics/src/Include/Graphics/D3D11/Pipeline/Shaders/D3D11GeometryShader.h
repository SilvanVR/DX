#pragma once
/**********************************************************************
    class: GeometryShader (D3D11GeometryShader.h)

    author: S. Hau
    date: June 14, 2018
**********************************************************************/

#include "D3D11ShaderBase.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class GeometryShader : public ShaderBase
    {
    public:
        GeometryShader() : ShaderBase( ShaderType::Geometry ) {}
        ~GeometryShader() = default;

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void bind() override;
        void unbind() override;
        void compileFromFile(const OS::Path& path, CString entryPoint) override;
        void compileFromSource(const String& shaderSource, CString entryPoint) override;

    private:
        ComPtr<ID3D11GeometryShader> m_pGeometryShader = nullptr;

        void _CreateD3D11GeometryShader(const ShaderBlob& shaderBlob);

        NULL_COPY_AND_ASSIGN(GeometryShader)
    };


} } // End namespaces