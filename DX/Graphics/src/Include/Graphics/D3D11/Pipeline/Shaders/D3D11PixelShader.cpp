#include "D3D11PixelShader.h"
/**********************************************************************
    class: PixelShader (D3D11PixelShader.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {
    
    //----------------------------------------------------------------------
    PixelShader::~PixelShader()
    {
        SAFE_RELEASE( m_pPixelShader );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void PixelShader::bind()
    {
        ASSERT( m_pPixelShader != nullptr );
        g_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 );
    }

    //----------------------------------------------------------------------
    bool PixelShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        bool compiled = _CompileFromFile<ID3D11PixelShader>( path, entryPoint );
        if (not compiled)
            return false;

        _CreateD3D11PixelShader();

        return true;
    }

    //----------------------------------------------------------------------
    bool PixelShader::compileFromSource( const String& source, CString entryPoint )
    {
        bool compiled = _CompileFromSource<ID3D11PixelShader>( source, entryPoint );
        if (not compiled)
            return false;

        _CreateD3D11PixelShader();

        return true;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void PixelShader::_CreateD3D11PixelShader()
    {
        SAFE_RELEASE( m_pPixelShader );
        HR( g_pDevice->CreatePixelShader( m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        // Shader blob + reflection data no longer needed
        SAFE_RELEASE( m_pShaderBlob );
        SAFE_RELEASE( m_pShaderReflection );
    }

} } // End namespaces