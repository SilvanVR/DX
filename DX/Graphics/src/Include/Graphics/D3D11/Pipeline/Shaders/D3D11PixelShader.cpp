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
    // Public
    //**********************************************************************

    //----------------------------------------------------------------------
    void PixelShader::bind()
    {
        ASSERT( m_pPixelShader != nullptr );
        g_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 );
    }

    //----------------------------------------------------------------------
    bool PixelShader::compile( const OS::Path& path, CString entryPoint )
    {
        bool compiled = _Compile<ID3D11PixelShader>( path, entryPoint );
        if (not compiled)
            return false;

        SAFE_RELEASE( m_pPixelShader );

        HR( g_pDevice->CreatePixelShader( m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        // Shader blob + reflection data no longer needed
        SAFE_RELEASE( m_pShaderBlob );
        SAFE_RELEASE( m_pShaderReflection );

        return true;
    }

    //----------------------------------------------------------------------
    bool PixelShader::compile( const String& source, CString entryPoint )
    {
        bool compiled = _Compile<ID3D11PixelShader>( source, entryPoint );
        if (not compiled)
            return false;

        SAFE_RELEASE( m_pPixelShader );

        HR( g_pDevice->CreatePixelShader( m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        // Shader blob + reflection data no longer needed
        SAFE_RELEASE( m_pShaderBlob );
        SAFE_RELEASE( m_pShaderReflection );

        return true;
    }


    //----------------------------------------------------------------------
    bool PixelShader::recompile()
    {
        // Recompilation only possible on shader which were compiled by path
        if ( not m_filePath.toString().empty() )
            return compile( m_filePath, m_entryPoint.c_str() );
        else
            WARN_RENDERING( "PixelShader::recompile(): Recompilation not possible because no filepath exists in this class!" );

        return false;
    }

} } // End namespaces