#include "D3D11Shader.h"
/**********************************************************************
    class: D3D11Shader (D3D11Shader.cpp)

    author: S. Hau
    date: December 3, 2017
**********************************************************************/

#include <d3dcompiler.h>

namespace Core { namespace Graphics { namespace D3D11 {


    //----------------------------------------------------------------------
    Shader::Shader( CString path )
        : m_filePath( path )
    {
    }

    //----------------------------------------------------------------------
    Shader::~Shader()
    {
        if (m_shaderBlob)
            SAFE_RELEASE( m_shaderBlob );
    }

    //----------------------------------------------------------------------
    bool Shader::_Compile( CString entryPoint, CString profile )
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    #ifdef _DEBUG
        flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
    #endif

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( m_filePath ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         entryPoint, profile, flags, 0, &m_shaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            WARN_RENDERING( "Failed to compile shader '" + m_filePath + "'." );

            if (errorBlob)
            {
                WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
                SAFE_RELEASE( errorBlob );
            }

            if (m_shaderBlob)
                SAFE_RELEASE( m_shaderBlob );

            return false;
        }

        return true;
    }


    //**********************************************************************
    // VERTEX SHADER
    //**********************************************************************

    //----------------------------------------------------------------------
    VertexShader::VertexShader( CString path )
        : Shader( path )
    {
    }

    //----------------------------------------------------------------------
    VertexShader::~VertexShader()
    {
        if(m_pVertexShader)
            SAFE_RELEASE( m_pVertexShader );
    }

    //----------------------------------------------------------------------
    bool VertexShader::compile( CString entryPoint )
    {
        bool compiled = _Compile( entryPoint, "vs_5_0" );
        if (not compiled)
            return false;

        // @TODO investigate class linkage
        HR( g_pDevice->CreateVertexShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_pVertexShader ) );

        return true;
    }

    //**********************************************************************
    // PIXEL SHADER
    //**********************************************************************

    //----------------------------------------------------------------------
    PixelShader::PixelShader( CString path )
        : Shader( path )
    {
    }
    
    //----------------------------------------------------------------------
    PixelShader::~PixelShader()
    {
        if (m_pPixelShader)
            SAFE_RELEASE( m_pPixelShader );
    }

    //----------------------------------------------------------------------
    bool PixelShader::compile( CString entryPoint )
    {
        bool compiled = _Compile( entryPoint, "ps_5_0" );
        if (not compiled)
            return false;

        // @TODO investigate class linkage
        HR( g_pDevice->CreatePixelShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        return true;
    }

} } } // End namespaces