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
        bool compile(const OS::Path& path, CString entryPoint) override;
        bool compile(const String& shaderSource, CString entryPoint) override;
        bool recompile() override;

    private:
        ID3D11PixelShader* m_pPixelShader = nullptr;

        //----------------------------------------------------------------------
        PixelShader(const PixelShader& other)               = delete;
        PixelShader& operator = (const PixelShader& other)  = delete;
        PixelShader(PixelShader&& other)                    = delete;
        PixelShader& operator = (PixelShader&& other)       = delete;
    };


} } // End namespaces