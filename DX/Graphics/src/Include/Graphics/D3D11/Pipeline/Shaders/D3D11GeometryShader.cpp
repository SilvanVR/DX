#include "D3D11GeometryShader.h"
/**********************************************************************
    class: GeometryShader (D3D11GeometryShader.cpp)

    author: S. Hau
    date: June 14, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {
    
    //----------------------------------------------------------------------
    GeometryShader::~GeometryShader()
    {
        SAFE_RELEASE( m_pGeometryShader );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void GeometryShader::bind()
    {
        ASSERT( m_pGeometryShader != nullptr );
        g_pImmediateContext->GSSetShader( m_pGeometryShader, NULL, 0 );
    }

    //----------------------------------------------------------------------
    void GeometryShader::unbind()
    {
        g_pImmediateContext->GSSetShader( NULL, NULL, 0 );
    }

    //----------------------------------------------------------------------
    bool GeometryShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        bool compiled = _CompileFromFile<ID3D11GeometryShader>( path, entryPoint );
        if (not compiled)
            return false;

        _CreateD3D11GeometryShader();

        return true;
    }

    //----------------------------------------------------------------------
    bool GeometryShader::compileFromSource( const String& source, CString entryPoint )
    {
        bool compiled = _CompileFromSource<ID3D11GeometryShader>( source, entryPoint );
        if (not compiled)
            return false;

        _CreateD3D11GeometryShader();

        return true;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void GeometryShader::_CreateD3D11GeometryShader()
    {
        SAFE_RELEASE( m_pGeometryShader );
        HR( g_pDevice->CreateGeometryShader( m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pGeometryShader ) );

        // Shader blob + reflection data no longer needed
        SAFE_RELEASE( m_pShaderBlob );
        SAFE_RELEASE( m_pShaderReflection );
    }

} } // End namespaces