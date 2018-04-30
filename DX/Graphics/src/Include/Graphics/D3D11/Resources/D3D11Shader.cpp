#include "D3D11Shader.h"
/**********************************************************************
    class: D3D11Shader (D3D11Shader.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../Pipeline/Shaders/D3D11VertexShader.h"
#include "../Pipeline/Shaders/D3D11PixelShader.h"
#include "../Resources/D3D11Mesh.h"
#include "../D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Shader::Shader()
    {
        _CreatePipeline();
    }

    //----------------------------------------------------------------------
    Shader::~Shader()
    {
        SAFE_RELEASE( m_pDepthStencilState );
        SAFE_RELEASE( m_pRSState );
        SAFE_RELEASE( m_pBlendState );
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        m_pVertexShader->bind();
        if (m_pPixelShader)
            m_pPixelShader->bind();

        g_pImmediateContext->OMSetDepthStencilState( m_pDepthStencilState, 0 );
        g_pImmediateContext->RSSetState( m_pRSState );
        g_pImmediateContext->OMSetBlendState( m_pBlendState ? m_pBlendState : NULL, m_blendFactors.data(), 0xffffffff );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    bool Shader::compileFromFile( const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint )
    {
        m_pVertexShader.reset( new D3D11::VertexShader() );
        m_pPixelShader.reset( new D3D11::PixelShader() );

        bool success = true;
        if ( not m_pVertexShader->compileFromFile( vertPath, entryPoint ) )
            success = false;
        if ( not m_pPixelShader->compileFromFile( fragPath, entryPoint ) )
            success = false;

        return success;
    }

    //----------------------------------------------------------------------
    bool Shader::compileFromSource( const String& vertSrc, const String& fragSrc, CString entryPoint )
    {
        bool success = true;
        if ( not compileVertexShaderFromSource( vertSrc, entryPoint ) )
            success = false;
        if ( not compileFragmentShaderFromSource( fragSrc, entryPoint ) )
            success = false;

        return success;
    }

    //----------------------------------------------------------------------
    bool Shader::compileVertexShaderFromSource( const String& src, CString entryPoint )
    {
        auto vertShader = new D3D11::VertexShader();
        if ( not vertShader->compileFromSource( src, entryPoint ) )
        {
            delete vertShader;
            return false;
        }

        m_pVertexShader.reset( vertShader );
        return true;
    }

    //----------------------------------------------------------------------
    bool Shader::compileFragmentShaderFromSource( const String& src, CString entryPoint )
    {
        auto pixelShader = new D3D11::PixelShader();
        if ( not pixelShader->compileFromSource( src, entryPoint ) )
        {
            delete pixelShader;
            return false;
        }

        m_pPixelShader.reset( pixelShader );
        return true;
    }

    //----------------------------------------------------------------------
    ArrayList<OS::Path> Shader::recompile()
    {
        ArrayList<OS::Path> shaderPaths;

        if (m_pVertexShader)
            if ( not m_pVertexShader->isUpToDate() )
                if ( m_pVertexShader->recompile() )
                    shaderPaths.emplace_back( m_pVertexShader->getFilePath() );
        if (m_pPixelShader)
            if ( not m_pPixelShader->isUpToDate() )
                if ( m_pPixelShader->recompile() )
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

    //----------------------------------------------------------------------
    const VertexLayout& Shader::getVertexLayout() const 
    { 
        return m_pVertexShader->getVertexLayout(); 
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
        case FillMode::Solid:       rsDesc.FillMode = D3D11_FILL_SOLID;     break;
        case FillMode::Wireframe:   rsDesc.FillMode = D3D11_FILL_WIREFRAME; break;
        }
        switch (rzState.cullMode)
        {
        case CullMode::Back:        rsDesc.CullMode = D3D11_CULL_BACK;      break;
        case CullMode::Front:       rsDesc.CullMode = D3D11_CULL_FRONT;     break;
        case CullMode::None:        rsDesc.CullMode = D3D11_CULL_NONE;      break;
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