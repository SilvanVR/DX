#pragma once
/**********************************************************************
    class: PixelShader (D3D11PixelShader.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "D3D11ShaderBase.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class PixelShader : public ShaderBase
    {
    public:
        PixelShader() : ShaderBase(ShaderType::Fragment) {}
        ~PixelShader() = default;

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void bind() override;
        void unbind() override;
        void compileFromFile(const OS::Path& path, CString entryPoint) override;
        void compileFromSource(const String& shaderSource, CString entryPoint) override;

    private:
        ComPtr<ID3D11PixelShader> m_pPixelShader = nullptr;

        void _CreateD3D11PixelShader(const ShaderBlob& shaderBlob);

        NULL_COPY_AND_ASSIGN(PixelShader)
    };


} } // End namespaces