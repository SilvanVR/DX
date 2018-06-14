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
        ~GeometryShader();

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void bind() override;
        void unbind() override;
        bool compileFromFile(const OS::Path& path, CString entryPoint) override;
        bool compileFromSource(const String& shaderSource, CString entryPoint) override;

    private:
        ID3D11GeometryShader* m_pGeometryShader = nullptr;

        void _CreateD3D11GeometryShader();

        NULL_COPY_AND_ASSIGN(GeometryShader)
    };


} } // End namespaces