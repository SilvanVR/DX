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
    Shader::Shader()
    {
        _CreatePipeline();
    }

    //----------------------------------------------------------------------
    Shader::~Shader()
    {
        if (m_pVertexShader)
            SAFE_DELETE( m_pVertexShader );
        if (m_pPixelShader)
            SAFE_DELETE( m_pPixelShader );

        SAFE_RELEASE( m_pDepthStencilState );
        SAFE_RELEASE( m_pRSState );
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        m_pVertexShader->bind();
        m_pPixelShader->bind();

        g_pImmediateContext->OMSetDepthStencilState( m_pDepthStencilState, 0 );
        g_pImmediateContext->RSSetState(m_pRSState );
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
    void Shader::setShaderPaths( const OS::Path& vertPath, const OS::Path& fragPath )
    {
        m_pVertexShader = new D3D11::VertexShader( vertPath );
        m_pPixelShader  = new D3D11::PixelShader( fragPath );
    }

    //----------------------------------------------------------------------
    bool Shader::compile( CString entryPoint )
    {
        bool success = true;
        if (m_pVertexShader)
            if (not m_pVertexShader->compile( entryPoint ))
                success = false;
        if (m_pPixelShader)
            if (not m_pPixelShader->compile( entryPoint ))
                success = false;

        return success;
    }

    //----------------------------------------------------------------------
    ArrayList<OS::Path> Shader::recompile()
    {
        ArrayList<OS::Path> shaderPaths;

        if (m_pVertexShader)
            if ( not m_pVertexShader->isUpToDate() )
                if ( m_pVertexShader->compile( m_pVertexShader->getEntryPoint() ) )
                    shaderPaths.emplace_back( m_pVertexShader->getFilePath() );
        if (m_pPixelShader)
            if ( not m_pPixelShader->isUpToDate() )
                if ( m_pPixelShader->compile( m_pPixelShader->getEntryPoint() ) )
                    shaderPaths.emplace_back( m_pPixelShader->getFilePath() );

        return shaderPaths;
    }

    //----------------------------------------------------------------------
    bool Shader::isUpToDate()
    {
        if (m_pVertexShader)
            if ( not m_pVertexShader->isUpToDate() )
                return false;
        if (m_pPixelShader)
            if ( not m_pPixelShader->isUpToDate() )
                return false;

        return true;
    }

    //----------------------------------------------------------------------
    ArrayList<OS::Path> Shader::getShaderPaths() const
    {
        ArrayList<OS::Path> shaderPaths;
        if (m_pVertexShader) shaderPaths.emplace_back( m_pVertexShader->getFilePath() );
        if (m_pPixelShader)  shaderPaths.emplace_back( m_pPixelShader->getFilePath() );
        return shaderPaths;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Shader::_CreatePipeline()
    {
        {
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};

            depthStencilStateDesc.DepthEnable = TRUE;
            depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilStateDesc.StencilEnable = FALSE;

            HR( g_pDevice->CreateDepthStencilState( &depthStencilStateDesc, &m_pDepthStencilState) );
        }

        setRasterizationState({});
    }

    //----------------------------------------------------------------------
    void Shader::setRasterizationState( const RasterizationState& rzState )
    {
        SAFE_RELEASE( m_pRSState );

        D3D11_RASTERIZER_DESC rsDesc = {};
        switch (rzState.FillMode)
        {
        case FillMode::SOLID:       rsDesc.FillMode = D3D11_FILL_SOLID;     break;
        case FillMode::WIREFRAME:   rsDesc.FillMode = D3D11_FILL_WIREFRAME; break;
        }
        switch (rzState.CullMode)
        {
        case CullMode::BACK:        rsDesc.CullMode = D3D11_CULL_BACK;      break;
        case CullMode::FRONT:       rsDesc.CullMode = D3D11_CULL_FRONT;     break;
        case CullMode::NONE:        rsDesc.CullMode = D3D11_CULL_NONE;  break;
        }

        rsDesc.FrontCounterClockwise = rzState.FrontCounterClockwise;
        rsDesc.DepthClipEnable   = true;
        rsDesc.MultisampleEnable = true;
        HR( g_pDevice->CreateRasterizerState( &rsDesc, &m_pRSState ) );
    }

} } // End namespaces