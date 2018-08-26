#include "VkShaderModule.h"
/**********************************************************************
    class: ShaderModule

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "Common/string_utils.h"
#include "OS/FileSystem/file_system.h"
#include "OS/FileSystem/file.h"
#include "Utils/utils.h"
#include "../VkUtility.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    ShaderModule::~ShaderModule()
    {
        vezDestroyShaderModule( g_vulkan.device, m_shaderModule );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ShaderModule::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        if ( not path.exists() )
            throw std::runtime_error( "Missing shader-file: '" + path.toString() + "'." );

        // Load file
        OS::BinaryFile binaryShaderFile( path, OS::EFileMode::READ );
        String content = binaryShaderFile.readAll();

        ArrayList<uint32_t> spv( content.begin(), content.end() );

        // Create shader module
        VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        createInfo.pCode    = spv.data();
        createInfo.codeSize = spv.size() * sizeof(uint32_t);
        vkCreateShaderModule( g_vulkan.device, &createInfo, ALLOCATOR, &m_shaderModule );
    }

    //----------------------------------------------------------------------
    void ShaderModule::compileFromSource( const String& source, CString entryPoint )
    {
        String shaderName = GetShaderTypeName( m_shaderType );

        StringID hash = SID( source.c_str() );

#ifdef _DEBUG
        OS::Path binaryShaderPath( "/engine/shaders/bin/debug/" + shaderName + TS( hash.id ) + ".spv" );
#else
        OS::Path binaryShaderPath( "/engine/shaders/bin/release/" + shaderName + TS( hash.id ) + ".spv" );
#endif

        // Precompiled binary file does not exist
        if ( not OS::FileSystem::exists( binaryShaderPath ) )
        {
            _CompileGLSL( source, entryPoint );

            // Store compiled binary data into file
            LOG_WARN_RENDERING( "VkShaderModule: Saving precompiled shaders on disk not supported yet." );
            /*OS::BinaryFile binaryShaderFile( binaryShaderPath, OS::EFileMode::WRITE );
            binaryShaderFile.write( (const Byte*)spv.data(), spv.size() * sizeof(uint32_t) );*/
        }
        else
        {
            // Load compiled binary data from file
            OS::BinaryFile binaryShaderFile( binaryShaderPath, OS::EFileMode::READ );
            String content = binaryShaderFile.readAll();

            // This should always work
            VezShaderModuleCreateInfo createInfo = {};
            createInfo.stage        = Utility::TranslateShaderStage( m_shaderType );
            createInfo.codeSize     = static_cast<U32>(source.size());
            createInfo.pGLSLSource  = source.c_str();
            createInfo.pEntryPoint  = entryPoint;
            vezCreateShaderModule( g_vulkan.device, &createInfo, &m_shaderModule );
        }
    }

    //----------------------------------------------------------------------
    void ShaderModule::_CompileGLSL( const String& source, CString entryPoint )
    {
        VkShaderStageFlagBits shaderStage = Utility::TranslateShaderStage( m_shaderType );
        if (not shaderStage)
            throw new std::runtime_error( "VkShaderModule: Unsupported shader type." );

        VezShaderModuleCreateInfo createInfo = {};
        createInfo.stage        = shaderStage;
        createInfo.codeSize     = static_cast<U32>( source.size() );
        createInfo.pGLSLSource  = source.c_str();
        createInfo.pEntryPoint  = entryPoint;

        auto result = vezCreateShaderModule( g_vulkan.device, &createInfo, &m_shaderModule );
        if (result != VK_SUCCESS && m_shaderModule != VK_NULL_HANDLE)
        {
            // If shader module creation failed get the error log.
            U32 infoLogSize = 0;
            vezGetShaderModuleInfoLog( m_shaderModule, &infoLogSize, nullptr );
            String infoLog( infoLogSize, '\0' );
            vezGetShaderModuleInfoLog( m_shaderModule, &infoLogSize, &infoLog[0] );

            vezDestroyShaderModule( g_vulkan.device, m_shaderModule );
            throw new std::runtime_error( infoLog );
        }
    }

} } // End namespaces