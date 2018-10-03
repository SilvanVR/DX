#include "D3D11Shader.h"
/**********************************************************************
    class: D3D11Shader (D3D11Shader.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../Pipeline/Shaders/D3D11VertexShader.h"
#include "../Pipeline/Shaders/D3D11PixelShader.h"
#include "../Pipeline/Shaders/D3D11GeometryShader.h"
#include "../D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Shader::Shader()
    {
        setDepthStencilState({});
        setRasterizationState({});
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        // Bind shaders
        m_pVertexShader->bind();
        if (m_pPixelShader)
            m_pPixelShader->bind();
        if (m_pGeometryShader)
            m_pGeometryShader->bind();

        // Bind constant buffers and textures
        if (m_shaderDataVS) m_shaderDataVS->bind( ShaderType::Vertex );
        if (m_shaderDataPS) m_shaderDataPS->bind( ShaderType::Fragment );
        if (m_shaderDataGS) m_shaderDataGS->bind( ShaderType::Geometry );
        _BindTextures();

        // Bind pipeline states
        g_pImmediateContext->OMSetDepthStencilState( m_pDepthStencilState, 0 );
        g_pImmediateContext->RSSetState( m_pRSState );

        if (m_pBlendState)
            g_pImmediateContext->OMSetBlendState( m_pBlendState, m_blendFactors.data(), 0xffffffff );
        else
            g_pImmediateContext->OMSetBlendState( NULL, m_blendFactors.data(), 0xffffffff );
    }

    //----------------------------------------------------------------------
    void Shader::unbind()
    {
        // Unbind shaders
        m_pVertexShader->unbind();
        if (m_pPixelShader)
            m_pPixelShader->unbind();
        if (m_pGeometryShader)
            m_pGeometryShader->unbind();

        g_pImmediateContext->OMSetDepthStencilState( NULL, 0 );
        g_pImmediateContext->RSSetState( NULL );
        g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xffffffff );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Shader::compileFromFile( const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint )
    {
        m_pVertexShader.reset( new D3D11::VertexShader() );
        m_pPixelShader.reset( new D3D11::PixelShader() );

        m_pVertexShader->compileFromFile( vertPath, entryPoint );
        m_pPixelShader->compileFromFile( fragPath, entryPoint );
    }

    //----------------------------------------------------------------------
    void Shader::compileFromSource( const String& vertSrc, const String& fragSrc, CString entryPoint )
    {
        compileVertexShaderFromSource( vertSrc, entryPoint );
        compileFragmentShaderFromSource( fragSrc, entryPoint );
    }

    //----------------------------------------------------------------------
    void Shader::compileVertexShaderFromSource( const String& src, CString entryPoint )
    {
        auto vertShader = std::make_unique<D3D11::VertexShader>();

        vertShader->compileFromSource( src, entryPoint );

        m_pVertexShader.swap( vertShader );
    }

    //----------------------------------------------------------------------
    void Shader::compileFragmentShaderFromSource( const String& src, CString entryPoint )
    {
        auto pixelShader = std::make_unique<D3D11::PixelShader>();
        pixelShader->compileFromSource( src, entryPoint );

        m_pPixelShader.swap( pixelShader );
    }

    //----------------------------------------------------------------------
    void Shader::compileGeometryShaderFromSource( const String& src, CString entryPoint )
    {
        auto geometryShader = std::make_unique<D3D11::GeometryShader>();
        geometryShader->compileFromSource( src, entryPoint );

        m_pGeometryShader.swap( geometryShader );
    }

    //----------------------------------------------------------------------
    const VertexLayout& Shader::getVertexLayout() const 
    { 
        return m_pVertexShader->getVertexLayout(); 
    }

    //----------------------------------------------------------------------
    void Shader::setRasterizationState( const RasterizationState& rzState )
    {
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
        rsDesc.DepthClipEnable       = rzState.depthClipEnable;
        rsDesc.MultisampleEnable     = true;
        rsDesc.ScissorEnable         = rzState.scissorEnable;
        rsDesc.DepthBias             = (INT)rzState.depthBias;
        rsDesc.SlopeScaledDepthBias  = rzState.slopeScaledDepthBias;
        rsDesc.DepthBiasClamp        = rzState.depthBiasClamp;

        HR( g_pDevice->CreateRasterizerState( &rsDesc, &m_pRSState.releaseAndGet() ) );
    }

    //----------------------------------------------------------------------
    void Shader::setDepthStencilState( const DepthStencilState& dsState )
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};

        depthStencilStateDesc.DepthEnable       = dsState.depthEnable;
        depthStencilStateDesc.DepthWriteMask    = dsState.depthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilStateDesc.DepthFunc         = Utility::TranslateComparisonFunc( dsState.depthFunc );

        HR( g_pDevice->CreateDepthStencilState( &depthStencilStateDesc, &m_pDepthStencilState.releaseAndGet() ) );
    }

    //----------------------------------------------------------------------
    void Shader::setBlendState( const BlendState& bState )
    {
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

        HR( g_pDevice->CreateBlendState( &blendDesc, &m_pBlendState.releaseAndGet() ) );
    }

    //----------------------------------------------------------------------
    void _AddUbos( ArrayList<ShaderUniformBufferDeclaration>& list, const ArrayList<ShaderUniformBufferDeclaration>& ubos )
    {
        for (auto& ubo : ubos)
        {
            // Check if ubo already exists.
            auto it = std::find( list.begin(), list.end(), ubo );
            if (it == list.end()) // Ubo didn't exist, add it
                list.push_back( ubo );
            else // Ubo already exist, add shader-stage
                it->_AddShaderStage( ubo.getShaderStages() );
        }
    }

    //----------------------------------------------------------------------
    void _AddShaderResource( ArrayList<ShaderResourceDeclaration>& list, const ArrayList<ShaderResourceDeclaration>& resources )
    {
        for (auto& res : resources)
        {
            // Check if ubo already exists.
            auto it = std::find( list.begin(), list.end(), res );
            if (it == list.end()) // Ubo didn't exist, add it
                list.push_back( res );
            else // Ubo already exist, add shader-stage
                it->_AddShaderStage( res.getShaderStages() );
        }
    }

    //----------------------------------------------------------------------
    void Shader::createPipeline()
    {
        m_uniformBuffers.clear();
        m_shaderResources.clear();

        if (m_pVertexShader)
        {
            auto ubos = m_pVertexShader->getConstantBufferBindings();
            _AddUbos( m_uniformBuffers, ubos );

            auto resources = m_pVertexShader->getResourceDeclarations();
            _AddShaderResource( m_shaderResources, resources );
        }
        if (m_pPixelShader)
        {
            auto ubos = m_pPixelShader->getConstantBufferBindings();
            _AddUbos( m_uniformBuffers, ubos );

            auto resources = m_pPixelShader->getResourceDeclarations();
            _AddShaderResource( m_shaderResources, resources );
        }
        if (m_pGeometryShader)
        {
            auto ubos = m_pGeometryShader->getConstantBufferBindings();
            _AddUbos( m_uniformBuffers, ubos );

            auto resources = m_pGeometryShader->getResourceDeclarations();
            _AddShaderResource( m_shaderResources, resources );
        }

        _CreateConstantBuffers();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Shader::_CreateConstantBuffers()
    {
        _ClearAllMaps();
        if (auto cb = getVSUniformShaderBuffer())
            m_shaderDataVS.reset( new MappedConstantBuffer( *cb, BufferUsage::LongLived ) );
        if (auto cb = getFSUniformShaderBuffer())
            m_shaderDataPS.reset( new MappedConstantBuffer( *cb, BufferUsage::LongLived ) );
        if (auto cb = getGSUniformShaderBuffer())
            m_shaderDataGS.reset( new MappedConstantBuffer( *cb, BufferUsage::LongLived ) );
    }

    //----------------------------------------------------------------------
    void Shader::_UpdateConstantBuffer( StringID name, const void* pData )
    {
        // Because the super shader class issues if the uniform does not exist,
        // i dont have to do it here. The update call on the corresponding mapped buffer
        // will do nothing if the name does not exist.
        if (m_shaderDataVS) m_shaderDataVS->update( name, pData );
        if (m_shaderDataPS) m_shaderDataPS->update( name, pData );
        if (m_shaderDataGS) m_shaderDataGS->update( name, pData );
    }

} } // End namespaces