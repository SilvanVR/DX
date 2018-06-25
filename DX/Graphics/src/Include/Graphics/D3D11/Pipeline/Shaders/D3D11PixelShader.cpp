#include "D3D11PixelShader.h"
/**********************************************************************
    class: PixelShader (D3D11PixelShader.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "OS/FileSystem/file_system.h"
#include "OS/FileSystem/file.h"

namespace Graphics { namespace D3D11 {
    
    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void PixelShader::bind()
    {
        ASSERT( m_pPixelShader.get() != nullptr );
        g_pImmediateContext->PSSetShader( m_pPixelShader.get(), NULL, 0 );
    }

    //----------------------------------------------------------------------
    void PixelShader::unbind()
    {
        g_pImmediateContext->PSSetShader( NULL, NULL, 0 );
    }

    //----------------------------------------------------------------------
    void PixelShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        _CompileFromFile( path, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateD3D11PixelShader(shaderBlob);
        } );
    }

    //----------------------------------------------------------------------
    void PixelShader::compileFromSource( const String& source, CString entryPoint )
    {
        _CompileFromSource( source, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateD3D11PixelShader( shaderBlob );
        } );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void PixelShader::_CreateD3D11PixelShader( const ShaderBlob& shaderBlob )
    {
        HR( g_pDevice->CreatePixelShader( shaderBlob.data, shaderBlob.size, nullptr, &m_pPixelShader.releaseAndGet() ) );

        // Shader blob and reflection data no longer needed
        m_pShaderReflection.release();
    }


} } // End namespaces