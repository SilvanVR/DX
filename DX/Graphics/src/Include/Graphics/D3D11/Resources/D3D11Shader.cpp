#include "D3D11Shader.h"
/**********************************************************************
    class: D3D11Shader (D3D11Shader.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../Pipeline/Shaders/D3D11Shaders.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Shader::~Shader()
    {
        if (pVertexShader)
            SAFE_DELETE( pVertexShader );
        if (pPixelShader)
            SAFE_DELETE( pPixelShader );
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        pVertexShader->bind();
        pPixelShader->bind();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Shader::setShaderPaths( CString vertPath, CString fragPath )
    {
        pVertexShader = new D3D11::VertexShader( vertPath );
        pPixelShader  = new D3D11::PixelShader( fragPath );
    }

    //----------------------------------------------------------------------
    void Shader::compile( CString entryPoint )
    {
        if (pVertexShader)
            pVertexShader->compile( entryPoint );
        if (pPixelShader)
            pPixelShader->compile( entryPoint );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

} } // End namespaces