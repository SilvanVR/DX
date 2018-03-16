#include "D3D11Shader.h"
/**********************************************************************
    class: D3D11Shader (D3D11Shader.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../Pipeline/Shaders/D3D11Shaders.h"
#include "../Resources/D3D11Mesh.h"
#include "../D3D11Utility.hpp"

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
        SAFE_RELEASE( m_pBlendState );
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        m_pVertexShader->bind();
        m_pPixelShader->bind();

        g_pImmediateContext->OMSetDepthStencilState( m_pDepthStencilState, 0 );
        g_pImmediateContext->RSSetState( m_pRSState );
        g_pImmediateContext->OMSetBlendState( m_pBlendState ? m_pBlendState : NULL, m_blendFactors.data(), 0xffffffff );
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
        setDepthStencilState({});
        setRasterizationState({});
    }

    //----------------------------------------------------------------------
    void Shader::setRasterizationState( const RasterizationState& rzState )
    {
        SAFE_RELEASE( m_pRSState );

        D3D11_RASTERIZER_DESC rsDesc = {};
        switch (rzState.fillMode)
        {
        case FillMode::SOLID:       rsDesc.FillMode = D3D11_FILL_SOLID;     break;
        case FillMode::WIREFRAME:   rsDesc.FillMode = D3D11_FILL_WIREFRAME; break;
        }
        switch (rzState.cullMode)
        {
        case CullMode::BACK:        rsDesc.CullMode = D3D11_CULL_BACK;      break;
        case CullMode::FRONT:       rsDesc.CullMode = D3D11_CULL_FRONT;     break;
        case CullMode::NONE:        rsDesc.CullMode = D3D11_CULL_NONE;      break;
        }

        rsDesc.FrontCounterClockwise = rzState.frontCounterClockwise;
        rsDesc.DepthClipEnable   = true;
        rsDesc.MultisampleEnable = true;
        HR( g_pDevice->CreateRasterizerState( &rsDesc, &m_pRSState ) );
    }

    //----------------------------------------------------------------------
    void Shader::setDepthStencilState( const DepthStencilState& dsState )
    {
        SAFE_RELEASE( m_pDepthStencilState );

        D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};

        depthStencilStateDesc.DepthEnable       = dsState.depthEnable;
        depthStencilStateDesc.DepthWriteMask    = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilStateDesc.DepthFunc         = Utility::TranslateComparisonFunc( dsState.depthFunc );

        HR( g_pDevice->CreateDepthStencilState( &depthStencilStateDesc, &m_pDepthStencilState) );
    }

    //----------------------------------------------------------------------
    void Shader::setBlendState( const BlendState& bState )
    {
        SAFE_RELEASE( m_pBlendState );

        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable  = bState.alphaToCoverage;
        blendDesc.IndependentBlendEnable = bState.independentBlending;

        for ( I32 i = 0; i < (bState.independentBlending ? 8 : 1); i++ )
        {
            auto& bs = bState.blendStates[i];
            blendDesc.RenderTarget[i].BlendEnable           = bs.blendEnable;
            blendDesc.RenderTarget[i].SrcBlend              = Utility::TranslateBlend( bs.srcBlend );
            blendDesc.RenderTarget[i].DestBlend             = Utility::TranslateBlend( bs.destBlend );
            blendDesc.RenderTarget[i].BlendOp               = Utility::TranslateBlendOP( bs.blendOp );
            blendDesc.RenderTarget[i].SrcBlendAlpha         = Utility::TranslateBlend( bs.srcBlendAlpha );
            blendDesc.RenderTarget[i].DestBlendAlpha        = Utility::TranslateBlend( bs.destBlendAlpha );
            blendDesc.RenderTarget[i].BlendOpAlpha          = Utility::TranslateBlendOP( bs.blendOpAlpha );
            blendDesc.RenderTarget[i].RenderTargetWriteMask = bs.writeMask;
        }

        HR( g_pDevice->CreateBlendState( &blendDesc, &m_pBlendState ) );
    }

} } // End namespaces