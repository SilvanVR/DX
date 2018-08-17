#include "VkShader.h"
/**********************************************************************
    class: Shader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "../Pipeline/Shaders/VkVertexShader.h"
#include "../Pipeline/Shaders/VkFragmentShader.h"
#include "../Pipeline/Shaders/VkGeometryShader.h"
//#include "../Resources/VkMesh.h"
#include "../VkUtility.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    Shader::Shader()
    {
        _CreatePipeline();
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        // Bind pipeline
        //g_vulkan.ctx.bindPipeline( m_pipeline );

        //// Bind constant buffers and textures
        //if (m_shaderDataVS) m_shaderDataVS->bind( ShaderType::Vertex );
        //if (m_shaderDataPS) m_shaderDataPS->bind( ShaderType::Fragment );
        //if (m_shaderDataGS) m_shaderDataGS->bind( ShaderType::Geometry );
        //_BindTextures();
    }

    //----------------------------------------------------------------------
    void Shader::unbind()
    {
        //g_vulkan.ctx.bindPipeline( VK_NULL_HANDLE );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Shader::compileFromFile( const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint )
    {
        m_pVertexShader.reset( new Vulkan::VertexShader() );
        m_pFragmentShader.reset( new Vulkan::FragmentShader() );

        m_pVertexShader->compileFromFile( vertPath, entryPoint );
        m_pFragmentShader->compileFromFile( fragPath, entryPoint );

        _CreateConstantBuffers();
    }

    //----------------------------------------------------------------------
    void Shader::compileFromSource( const String& vertSrc, const String& fragSrc, CString entryPoint )
    {
        compileVertexShaderFromSource( vertSrc, entryPoint );
        compileFragmentShaderFromSource( fragSrc, entryPoint );
        _CreateConstantBuffers();
    }

    //----------------------------------------------------------------------
    void Shader::compileVertexShaderFromSource( const String& src, CString entryPoint )
    {
        auto vertShader = std::make_unique<Vulkan::VertexShader>();

        vertShader->compileFromSource( src, entryPoint );

        m_pVertexShader.swap( vertShader );
        _CreateVSConstantBuffer();
    }

    //----------------------------------------------------------------------
    void Shader::compileFragmentShaderFromSource( const String& src, CString entryPoint )
    {
        auto pixelShader = std::make_unique<Vulkan::FragmentShader>();
        pixelShader->compileFromSource( src, entryPoint );

        m_pFragmentShader.swap( pixelShader );
        _CreatePSConstantBuffer();
    }

    //----------------------------------------------------------------------
    void Shader::compileGeometryShaderFromSource( const String& src, CString entryPoint )
    {
        auto geometryShader = std::make_unique<Vulkan::GeometryShader>();
        geometryShader->compileFromSource( src, entryPoint );

        m_pGeometryShader.swap( geometryShader );
        _CreateGSConstantBuffer();
    }

    //----------------------------------------------------------------------
    const VertexLayout& Shader::getVertexLayout() const 
    { 
        return m_pVertexShader->getVertexLayout(); 
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getVSUniformMaterialBuffer() const 
    {
        return m_pVertexShader->getMaterialBufferDeclaration();
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getFSUniformMaterialBuffer() const 
    { 
        return m_pFragmentShader->getMaterialBufferDeclaration();
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getGSUniformMaterialBuffer() const 
    { 
        return m_pGeometryShader->getMaterialBufferDeclaration();
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getVSUniformShaderBuffer() const
    {
        return m_pVertexShader->getShaderBufferDeclaration();
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getFSUniformShaderBuffer() const
    {
        return m_pFragmentShader->getShaderBufferDeclaration();
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getGSUniformShaderBuffer() const
    {
        return m_pGeometryShader->getShaderBufferDeclaration();
    }

    //----------------------------------------------------------------------
    const ShaderResourceDeclaration* Shader::getShaderResource( StringID name ) const
    {
        auto decl1 = m_pVertexShader->getResourceDeclaration( name );
        auto decl2 = m_pFragmentShader ? m_pFragmentShader->getResourceDeclaration( name ) : nullptr;
        auto decl3 = m_pGeometryShader ? m_pGeometryShader->getResourceDeclaration( name ) : nullptr;

        if ( decl1 && decl2 && decl3 )
            LOG_WARN_RENDERING( "Shader::getShaderResource(): Resource with name '" + name.toString() + "' exists in more than one shader." );

        if (decl1)
            return decl1;
        else if (decl2)
            return decl2;
        else if (decl3)
            return decl3;

        // Not found
        return nullptr;
    }

    //----------------------------------------------------------------------
    void Shader::setRasterizationState( const RasterizationState& rzState )
    {
        switch (rzState.fillMode)
        {
        case FillMode::Solid:       m_rzState.polygonMode = VK_POLYGON_MODE_FILL; break;
        case FillMode::Wireframe:   m_rzState.polygonMode = VK_POLYGON_MODE_LINE; break;
        }
        switch (rzState.cullMode)
        {
        case CullMode::Back:        m_rzState.cullMode = VK_CULL_MODE_BACK_BIT; break;
        case CullMode::Front:       m_rzState.cullMode = VK_CULL_MODE_FRONT_BIT; break;
        case CullMode::None:        m_rzState.cullMode = VK_CULL_MODE_NONE; break;
        }

        m_rzState.depthClampEnable          = rzState.depthClipEnable;
        m_rzState.rasterizerDiscardEnable   = VK_FALSE;
        m_rzState.frontFace                 = rzState.frontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
        m_rzState.depthBiasEnable           = rzState.depthBias == 0.0f ? VK_FALSE : VK_TRUE;
        m_rzState.depthBiasConstantFactor   = rzState.depthBias;
        m_rzState.depthBiasClamp            = rzState.depthBiasClamp;
        m_rzState.depthBiasSlopeFactor      = rzState.slopeScaledDepthBias;
        m_rzState.lineWidth                 = 1.0f;
    }

    //----------------------------------------------------------------------
    void Shader::setDepthStencilState( const DepthStencilState& dsState )
    {
        m_depthStencilState.depthTestEnable  = dsState.depthEnable;
        m_depthStencilState.depthWriteEnable = dsState.depthWrite;
        m_depthStencilState.depthCompareOp = Utility::TranslateComparisonFunc( dsState.depthFunc );

        m_depthStencilState.depthBoundsTestEnable = VK_FALSE;
        m_depthStencilState.stencilTestEnable = VK_FALSE;
        m_depthStencilState.minDepthBounds = 0.0f;
        m_depthStencilState.maxDepthBounds = 1.0f;
    }

    //----------------------------------------------------------------------
    void Shader::setBlendState( const BlendState& bState )
    {
        ASSERT( not bState.independentBlending && "Not supported" );
        ASSERT( not bState.alphaToCoverage && "Not supported" );

        auto& bs = bState.blendStates[0];
        m_blendState.blendEnable         = bs.blendEnable;
        m_blendState.srcColorBlendFactor = Utility::TranslateBlend( bs.srcBlend );
        m_blendState.dstColorBlendFactor = Utility::TranslateBlend( bs.destBlend );
        m_blendState.colorBlendOp        = Utility::TranslateBlendOP( bs.blendOp );
        m_blendState.srcAlphaBlendFactor = Utility::TranslateBlend( bs.srcBlendAlpha );;
        m_blendState.dstAlphaBlendFactor = Utility::TranslateBlend( bs.destBlendAlpha );;
        m_blendState.alphaBlendOp        = Utility::TranslateBlendOP( bs.blendOpAlpha );
        m_blendState.colorWriteMask      = bs.writeMask;
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
    void Shader::_CreateVSConstantBuffer()
    {
        _ClearAllMaps();
        //if ( auto cb = getVSUniformShaderBuffer() )
        //    m_shaderDataVS.reset( new MappedConstantBuffer( *cb, BufferUsage::LongLived ) );
    }

    //----------------------------------------------------------------------
    void Shader::_CreatePSConstantBuffer()
    {
        _ClearAllMaps();
        //if ( auto cb = getFSUniformShaderBuffer() )
        //    m_shaderDataPS.reset( new MappedConstantBuffer( *cb, BufferUsage::LongLived ) );
    }

    //----------------------------------------------------------------------
    void Shader::_CreateGSConstantBuffer()
    {
        _ClearAllMaps();
        //if ( auto cb = getGSUniformShaderBuffer() )
        //    m_shaderDataGS.reset( new MappedConstantBuffer( *cb, BufferUsage::LongLived ) );
    }

    //----------------------------------------------------------------------
    void Shader::_CreateConstantBuffers()
    {
        _CreateVSConstantBuffer();
        if (m_pFragmentShader)
            _CreatePSConstantBuffer();
        if (m_pGeometryShader)
            _CreateGSConstantBuffer();
    }

    //----------------------------------------------------------------------
    void Shader::_UpdateConstantBuffer( StringID name, const void* pData )
    {
        // Because the super shader class issues if the uniform does not exist,
        // i dont have to do it here. The update call on the corresponding mapped buffer
        // will do nothing if the name does not exist.
        //if (m_shaderDataVS) m_shaderDataVS->update( name, pData );
        //if (m_shaderDataPS) m_shaderDataPS->update( name, pData );
        //if (m_shaderDataGS) m_shaderDataGS->update( name, pData );
    }

} } // End namespaces