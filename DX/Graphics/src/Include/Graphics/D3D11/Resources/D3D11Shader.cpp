#include "D3D11Shader.h"
/**********************************************************************
    class: D3D11Shader (D3D11Shader.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../Pipeline/Shaders/D3D11Shaders.h"
#include "../Resources/D3D11Mesh.h"

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

    //----------------------------------------------------------------------
    void Shader::drawMesh( IMesh* mesh, U32 subMeshIndex )
    {
        // Bind buffers
        //m_pVertexBuffer->bind(0, sizeof(Math::Vec3), 0);
        //if (m_pColorBuffer != nullptr)
        //    m_pColorBuffer->bind(1, sizeof(F32) * 4, 0);

        //U32 strides[] = { sizeof(Math::Vec3), sizeof(F32) * 4 };
        //U32 offsets[] = { 0,0 };
        //ID3D11Buffer* pBuffers[] = { m_pVertexBuffer->getBuffer(), m_pColorBuffer->getBuffer() };
        //g_pImmediateContext->IASetVertexBuffers(0, 2, pBuffers, strides, offsets);

        // Submit draw call
        g_pImmediateContext->DrawIndexed( mesh->getIndexCount( subMeshIndex ), 0, mesh->getBaseVertex( subMeshIndex ) );
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
    bool Shader::compile( CString entryPoint )
    {
        bool success = true;
        if (pVertexShader)
            if (not pVertexShader->compile( entryPoint ))
                success = false;
        if (pPixelShader)
            if (not pPixelShader->compile( entryPoint ))
                success = false;

        return success;
    }

    //----------------------------------------------------------------------
    bool Shader::isUpToDate()
    {
        if (pVertexShader)
            if ( not pVertexShader->isUpToDate() )
                return false;
        if (pPixelShader)
            if ( not pPixelShader->isUpToDate() )
                return false;

        return true;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

} } // End namespaces