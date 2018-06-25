#include "D3D11GeometryShader.h"
/**********************************************************************
    class: GeometryShader (D3D11GeometryShader.cpp)

    author: S. Hau
    date: June 14, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void GeometryShader::bind()
    {
        ASSERT( m_pGeometryShader.get() != nullptr );
        g_pImmediateContext->GSSetShader( m_pGeometryShader.get(), NULL, 0 );
    }

    //----------------------------------------------------------------------
    void GeometryShader::unbind()
    {
        g_pImmediateContext->GSSetShader( NULL, NULL, 0 );
    }

    //----------------------------------------------------------------------
    void GeometryShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        _CompileFromFile( path, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateD3D11GeometryShader( shaderBlob );
        } );
    }

    //----------------------------------------------------------------------
    void GeometryShader::compileFromSource( const String& source, CString entryPoint )
    {
        _CompileFromSource( source, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateD3D11GeometryShader( shaderBlob );
        });
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void GeometryShader::_CreateD3D11GeometryShader( const ShaderBlob& shaderBlob )
    {
        HR( g_pDevice->CreateGeometryShader( shaderBlob.data, shaderBlob.size, nullptr, &m_pGeometryShader.releaseAndGet() ) );

        // Shader blob and reflection data no longer needed
        m_pShaderReflection.release();
    }

} } // End namespaces