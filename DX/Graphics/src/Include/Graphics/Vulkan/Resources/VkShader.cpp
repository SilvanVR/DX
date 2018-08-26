#include "VkShader.h"
/**********************************************************************
    class: Shader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "../Pipeline/VkShaderModule.h"
#include "../VkUtility.h"

// Input ending with this are treated as instance attributes
#define SEMANTIC_INSTANCED "_INSTANCE"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    Shader::Shader()
    {
        setDepthStencilState({});
        setRasterizationState({});
    }

    //----------------------------------------------------------------------
    Shader::~Shader()
    {
        vezDestroyPipeline( g_vulkan.device, m_pipeline );
        vezDestroyVertexInputFormat( g_vulkan.device, m_vertexInputFormat );
    }

    //----------------------------------------------------------------------
    void Shader::bind()
    {
        if (m_pipeline == VK_NULL_HANDLE)
        {
            _CreatePipeline();
            _PipelineResourceReflection( m_pipeline );
        }

        g_vulkan.ctx.SetVertexShader( m_pVertexShader->getVkShaderModule() );
        //g_vulkan.ctx.IASetInputLayout( m_pVertexShader->getVkInputLayout() );
        g_vulkan.ctx.OMSetBlendState( 0, m_blendState );
        g_vulkan.ctx.OMSetDepthStencilState( m_depthStencilState );
        g_vulkan.ctx.RSSetState( m_rzState );

        if (m_pFragmentShader)
            g_vulkan.ctx.SetFragmentShader( m_pFragmentShader->getVkShaderModule() );
        if (m_pGeometryShader)
            g_vulkan.ctx.SetGeometryShader( m_pGeometryShader->getVkShaderModule() );

        //// Bind constant buffers and textures
        //if (m_shaderDataVS) m_shaderDataVS->bind( ShaderType::Vertex );
        //if (m_shaderDataPS) m_shaderDataPS->bind( ShaderType::Fragment );
        //if (m_shaderDataGS) m_shaderDataGS->bind( ShaderType::Geometry );
        //_BindTextures();

        // Create descriptor-set layouts from shaders
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
        m_pVertexShader.reset( new Vulkan::ShaderModule( ShaderType::Vertex ) );
        m_pFragmentShader.reset( new Vulkan::ShaderModule( ShaderType::Fragment ) );

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
        auto vertShader = std::make_unique<Vulkan::ShaderModule>( ShaderType::Vertex );

        vertShader->compileFromSource( src, entryPoint );

        m_pVertexShader.swap( vertShader );
        _CreateVSConstantBuffer();
    }

    //----------------------------------------------------------------------
    void Shader::compileFragmentShaderFromSource( const String& src, CString entryPoint )
    {
        auto pixelShader = std::make_unique<Vulkan::ShaderModule>( ShaderType::Fragment );
        pixelShader->compileFromSource( src, entryPoint );

        m_pFragmentShader.swap( pixelShader );
        _CreatePSConstantBuffer();
    }

    //----------------------------------------------------------------------
    void Shader::compileGeometryShaderFromSource( const String& src, CString entryPoint )
    {
        auto geometryShader = std::make_unique<Vulkan::ShaderModule>( ShaderType::Geometry );
        geometryShader->compileFromSource( src, entryPoint );

        m_pGeometryShader.swap( geometryShader );
        _CreateGSConstantBuffer();
    }

    //----------------------------------------------------------------------
    const VertexLayout& Shader::getVertexLayout() const 
    { 
        return m_vertexLayout;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getVSUniformMaterialBuffer() const 
    {
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getFSUniformMaterialBuffer() const 
    { 
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getGSUniformMaterialBuffer() const 
    { 
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getVSUniformShaderBuffer() const
    {
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getFSUniformShaderBuffer() const
    {
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* Shader::getGSUniformShaderBuffer() const
    {
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderResourceDeclaration* Shader::getShaderResource( StringID name ) const
    {
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
        // Add shader modules
        ArrayList<VezPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        VezPipelineShaderStageCreateInfo stageCreateInfo{};
        if (m_pVertexShader)
        {
            stageCreateInfo.module = m_pVertexShader->getVkShaderModule();
            shaderStageCreateInfos.push_back( stageCreateInfo );
        }
        if (m_pFragmentShader)
        {
            stageCreateInfo.module = m_pFragmentShader->getVkShaderModule();
            shaderStageCreateInfos.push_back( stageCreateInfo );
        }
        if (m_pGeometryShader)
        {
            stageCreateInfo.module = m_pGeometryShader->getVkShaderModule();
            shaderStageCreateInfos.push_back( stageCreateInfo );
        }

        // Create pipeline
        VezGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.stageCount = static_cast<uint32_t>( shaderStageCreateInfos.size() );
        pipelineCreateInfo.pStages    = shaderStageCreateInfos.data();
        VALIDATE( vezCreateGraphicsPipeline( g_vulkan.device, &pipelineCreateInfo, &m_pipeline ) );
    }

    //----------------------------------------------------------------------
    void Shader::_PipelineResourceReflection( VezPipeline pipeline )
    {
        U32 resourceCount;
        vezEnumeratePipelineResources( pipeline, &resourceCount, NULL );
        ArrayList<VezPipelineResource> resources( resourceCount );
        vezEnumeratePipelineResources( pipeline, &resourceCount, resources.data() );

        _CreateVertexLayout( resources );
        _CreateShaderResources( resources );
    }

    //----------------------------------------------------------------------
    // @Return: VkFormat and sizeInBytes for the corresponding resource.
    //----------------------------------------------------------------------
    std::pair<VkFormat, U32> GetTypeInfo( const VezPipelineResource& resource )
    {
        switch (resource.baseType)
        {
        case VEZ_PIPELINE_RESOURCE_BASE_TYPE_INT:
        {
            switch (resource.vecSize)
            {
            case 1: return { VK_FORMAT_R32_SINT, 4 };
            case 2: return { VK_FORMAT_R32G32_SINT, 8 };
            case 3: return { VK_FORMAT_R32G32B32_SINT, 12 };
            case 4: return { VK_FORMAT_R32G32B32A32_SINT, 16 };
            }
        }
        case VEZ_PIPELINE_RESOURCE_BASE_TYPE_UINT:
        {
            switch (resource.vecSize)
            {
            case 1: return { VK_FORMAT_R32_UINT, 4 };
            case 2: return { VK_FORMAT_R32G32_UINT, 8 };
            case 3: return { VK_FORMAT_R32G32B32_UINT, 12 };
            case 4: return { VK_FORMAT_R32G32B32A32_UINT, 16 };
            }
        }
        case VEZ_PIPELINE_RESOURCE_BASE_TYPE_FLOAT:
        {
            switch (resource.vecSize)
            {
            case 1: return { VK_FORMAT_R32_SFLOAT, 4 };
            case 2: return { VK_FORMAT_R32G32_SFLOAT, 8 };
            case 3: return { VK_FORMAT_R32G32B32_SFLOAT, 12 };
            case 4: return { VK_FORMAT_R32G32B32A32_SFLOAT, 16 };
            }
        }
        case VEZ_PIPELINE_RESOURCE_BASE_TYPE_DOUBLE:
        {
            switch (resource.vecSize)
            {
            case 1: return { VK_FORMAT_R64_SFLOAT, 8 };
            case 2: return { VK_FORMAT_R64G64_SFLOAT, 16 };
            case 3: return { VK_FORMAT_R64G64B64_SFLOAT, 24 };
            case 4: return { VK_FORMAT_R64G64B64A64_SFLOAT, 32 };
            }
            break;
        }
        }
        LOG_WARN_RENDERING( "VkShader::_CreateVertexLayout(): Unknown vertex inputs." );
        return { VK_FORMAT_UNDEFINED, 0 };
    }

    //----------------------------------------------------------------------
    void Shader::_CreateVertexLayout( const ArrayList<VezPipelineResource>& resources )
    {
        ArrayList<VkVertexInputBindingDescription>      bindingDescriptions;
        ArrayList<VkVertexInputAttributeDescription>    attribDescriptions;
        for (auto& res : resources)
        {
            if (not (res.stages & VK_SHADER_STAGE_VERTEX_BIT)) // Skip fragment input
                continue;

            switch (res.resourceType)
            {
            case VEZ_PIPELINE_RESOURCE_TYPE_INPUT:
            {
                String name = res.name;
                Size pos = name.find( SEMANTIC_INSTANCED );
                constexpr Size sizeOfInstancedName = (sizeof(SEMANTIC_INSTANCED) / sizeof(char)) - 1;
                Size posIfNameIsAtEnd = name.size() - sizeOfInstancedName;
                bool instanced = (pos != String::npos) && (pos == posIfNameIsAtEnd);

                if (instanced)  // Cut-off the "SEMANTIC_INSTANCED"
                    name = name.substr( 0, pos );

                auto [format, sizeInBytes] = GetTypeInfo( res );

                VkVertexInputAttributeDescription attrDesc{};
                attrDesc.binding    = res.binding;
                attrDesc.location   = res.location;
                attrDesc.format     = format;
                attribDescriptions.push_back( attrDesc );

                VkVertexInputBindingDescription bindingDesc{};
                bindingDesc.inputRate = instanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
                bindingDesc.binding   = res.binding;
                bindingDesc.stride    = sizeInBytes;
                bindingDescriptions.push_back( bindingDesc );

                m_vertexLayout.add( { SID( name.c_str() ), sizeInBytes, instanced } );
                break;
            }
            }
        }
        VezVertexInputFormatCreateInfo vertexInputFormatCreateInfo = {};
        vertexInputFormatCreateInfo.vertexBindingDescriptionCount   = static_cast<U32>( bindingDescriptions.size() );
        vertexInputFormatCreateInfo.pVertexBindingDescriptions      = bindingDescriptions.data();
        vertexInputFormatCreateInfo.vertexAttributeDescriptionCount = static_cast<U32>( attribDescriptions.size() );
        vertexInputFormatCreateInfo.pVertexAttributeDescriptions    = attribDescriptions.data();
        VALIDATE( vezCreateVertexInputFormat( g_vulkan.device, &vertexInputFormatCreateInfo, &m_vertexInputFormat ) );
    }

    //----------------------------------------------------------------------
    void Shader::_CreateShaderResources( const ArrayList<VezPipelineResource>& resources )
    {
        for (auto& res : resources)
        {
            switch (res.resourceType)
            {
            case VEZ_PIPELINE_RESOURCE_TYPE_UNIFORM_BUFFER:
            {
                ShaderUniformBufferDeclaration ubo( SID(res.name), res.set, res.binding, res.size );
                // @TODO: Parse members
                m_uniformBuffers.push_back( ubo );
                break;
            }
            case VEZ_PIPELINE_RESOURCE_TYPE_SAMPLED_IMAGE:
            case VEZ_PIPELINE_RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER:
                ShaderType shaderType = ShaderType::Unknown;
                m_shaderResources.emplace_back( shaderType, res.set, res.binding, SID(res.name), DataType::Texture2D );
                break;
            }
        }
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