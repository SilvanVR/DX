#include "D3D11PixelShader.h"
/**********************************************************************
    class: PixelShader (D3D11PixelShader.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

//#include <d3dcompiler.h>

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    PixelShader::PixelShader( CString path )
        : ShaderBase( path )
    {
    }
    
    //----------------------------------------------------------------------
    PixelShader::~PixelShader()
    {
        SAFE_RELEASE( m_pPixelShader );
    }

    //**********************************************************************
    // Public
    //**********************************************************************

    //----------------------------------------------------------------------
    void PixelShader::bind()
    {
        ASSERT( m_pPixelShader != nullptr );
        g_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 );
    }

    //----------------------------------------------------------------------
    bool PixelShader::compile( CString entryPoint )
    {
        bool compiled = _Compile<ID3D11PixelShader>( entryPoint );
        if (not compiled)
            return false;

        SAFE_RELEASE( m_pPixelShader );

        HR( g_pDevice->CreatePixelShader( m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        // Shader blob + reflection data no longer needed
        SAFE_RELEASE( m_pShaderBlob );
        SAFE_RELEASE( m_pShaderReflection );

        return true;
    }

} } // End namespaces