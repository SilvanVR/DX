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
        PixelShader() = default;
        ~PixelShader();

        //----------------------------------------------------------------------
        // ShaderBase Interface
        //----------------------------------------------------------------------
        void bind() override;
        bool compileFromFile(const OS::Path& path, CString entryPoint) override;
        bool compileFromSource(const String& shaderSource, CString entryPoint) override;

    private:
        ID3D11PixelShader* m_pPixelShader = nullptr;

        void _CreateD3D11PixelShader();

        //----------------------------------------------------------------------
        PixelShader(const PixelShader& other)               = delete;
        PixelShader& operator = (const PixelShader& other)  = delete;
        PixelShader(PixelShader&& other)                    = delete;
        PixelShader& operator = (PixelShader&& other)       = delete;
    };


} } // End namespaces