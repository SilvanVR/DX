#include "D3D11Shader.h"
/**********************************************************************
    class: IShader + VertexShader etc. (D3D11IShader.cpp)

    author: S. Hau
    date: December 3, 2017
**********************************************************************/

#include <d3dcompiler.h>

namespace Core { namespace Graphics { namespace D3D11 {


    //----------------------------------------------------------------------
    IShader::IShader( CString path )
        : m_filePath( path )
    {
    }

    //----------------------------------------------------------------------
    IShader::~IShader()
    {
        SAFE_RELEASE( m_IShaderBlob );
    }

    //----------------------------------------------------------------------
    bool IShader::_Compile( CString entryPoint, CString profile )
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    #ifdef _DEBUG
        flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
    #endif

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( m_filePath ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         entryPoint, profile, flags, 0, &m_IShaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            WARN_RENDERING( "Failed to compile Shader '" + m_filePath + "'." );

            if (errorBlob)
            {
                WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
                SAFE_RELEASE( errorBlob );
            }

            SAFE_RELEASE( m_IShaderBlob );

            return false;
        }

        return true;
    }


    //**********************************************************************
    // VERTEX IShader
    //**********************************************************************

    //----------------------------------------------------------------------
    VertexShader::VertexShader( CString path )
        : IShader( path )
    {
    }

    //----------------------------------------------------------------------
    VertexShader::~VertexShader()
    {
        SAFE_RELEASE( m_pVertexShader );
    }

    //----------------------------------------------------------------------
    void VertexShader::bind() 
    { 
        ASSERT( m_pVertexShader != nullptr );
        g_pImmediateContext->VSSetShader( m_pVertexShader, NULL, 0 ); 
    }

    //----------------------------------------------------------------------
    bool VertexShader::compile( CString entryPoint )
    {
        bool compiled = _Compile( entryPoint, "vs_5_0" );
        if (not compiled)
            return false;

        SAFE_RELEASE( m_pVertexShader );

        HR( g_pDevice->CreateVertexShader( m_IShaderBlob->GetBufferPointer(), m_IShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader ) );

        return true;
    }

    //**********************************************************************
    // PIXEL IShader
    //**********************************************************************

    //----------------------------------------------------------------------
    PixelShader::PixelShader( CString path )
        : IShader( path )
    {
    }
    
    //----------------------------------------------------------------------
    PixelShader::~PixelShader()
    {
        SAFE_RELEASE( m_pPixelShader );
    }

    //----------------------------------------------------------------------
    void PixelShader::bind()
    {
        ASSERT( m_pPixelShader != nullptr );
        g_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 );
    }

    //----------------------------------------------------------------------
    bool PixelShader::compile( CString entryPoint )
    {
        bool compiled = _Compile( entryPoint, "ps_5_0" );
        if (not compiled)
            return false;

        SAFE_RELEASE( m_pPixelShader );

        HR( g_pDevice->CreatePixelShader( m_IShaderBlob->GetBufferPointer(), m_IShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        return true;
    }

} } } // End namespaces