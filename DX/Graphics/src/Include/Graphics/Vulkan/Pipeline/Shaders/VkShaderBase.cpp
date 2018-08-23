#include "VkShaderBase.h"
/**********************************************************************
    class: ShaderBase

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#define MATERIAL_NAME "material"
#define SHADER_NAME   "shader"

#include "Common/string_utils.h"
#include "OS/FileSystem/file_system.h"
#include "OS/FileSystem/file.h"
#include "Utils/utils.h"

#include <shaderc/shaderc.hpp>       // Compile glsl to spv
#include "SpirvCross/spirv_glsl.hpp" // spv reflection

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    ArrayList<uint32_t>                 CompileGLSLToSPV(const String& source, ShaderType shaderType, bool debug);
    DataType                            GetDataTypeOfMember(const spirv_cross::SPIRType& memberType);
    ArrayList<ShaderUniformDeclaration> ParseUniformBuffer(const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& spirType, U32 parentOffset, const String& parentName);

    //----------------------------------------------------------------------
    ShaderBase::~ShaderBase()
    {
        vkDestroyShaderModule( g_vulkan.device, m_shaderModule, ALLOCATOR );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* ShaderBase::getUniformBufferDeclaration( StringID name ) const
    {
        auto it = std::find_if( m_uniformBuffers.begin(), m_uniformBuffers.end(), [name](const ShaderUniformBufferDeclaration& ubo) {
            return ubo.getName() == name;
        } );
        if ( it == m_uniformBuffers.end() )
            return nullptr;

        return &(*it);
    }

    //----------------------------------------------------------------------
    const ShaderResourceDeclaration* ShaderBase::getResourceDeclaration( StringID name ) const
    {
        for (auto& decl : m_resourceDeclarations)
        {
            if (decl.getName() == name)
                return &decl;
        }
        return nullptr;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ShaderBase::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        if ( not path.exists() )
            throw std::runtime_error( "Missing shader-file: '" + path.toString() + "'." );

        // Load file
        OS::BinaryFile binaryShaderFile( path, OS::EFileMode::READ );
        String content = binaryShaderFile.readAll();

        ArrayList<uint32_t> spv( content.begin(), content.end() );
        _ShaderReflection( spv );

        // Create shader module
        VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        createInfo.pCode = spv.data();
        createInfo.codeSize = spv.size() * sizeof(uint32_t);
        vkCreateShaderModule( g_vulkan.device, &createInfo, ALLOCATOR, &m_shaderModule );
    }

    //----------------------------------------------------------------------
    void ShaderBase::compileFromSource( const String& source, CString entryPoint )
    {
        String shaderName = GetShaderTypeName( m_shaderType );

        StringID hash = SID( source.c_str() );

#ifdef _DEBUG
        OS::Path binaryShaderPath( "/engine/shaders/bin/debug/" + shaderName + TS( hash.id ) + ".spv" );
#else
        OS::Path binaryShaderPath( "/engine/shaders/bin/release/" + shaderName + TS( hash.id ) + ".spv" );
#endif

        // Precompiled binary file does not exist
        ArrayList<uint32_t> spv;
        if ( not OS::FileSystem::exists( binaryShaderPath ) )
        {
#ifdef _DEBUG
            spv = CompileGLSLToSPV( source, m_shaderType, true );
#else
            spv = CompileGLSLToSPV( source, m_shaderType, false );
#endif
            // Store compiled binary data into file
            OS::BinaryFile binaryShaderFile( binaryShaderPath, OS::EFileMode::WRITE );
            binaryShaderFile.write( (const Byte*)spv.data(), spv.size() * sizeof(uint32_t) );
        }
        else
        {
            // Load compiled binary data from file
            OS::BinaryFile binaryShaderFile( binaryShaderPath, OS::EFileMode::READ );
            String content = binaryShaderFile.readAll();

            spv.resize( content.size() / 4 ); // Array is of type uint32_t (4 bytes)
            memcpy( spv.data(), content.data(), content.size() ); // Copy raw bytes into Array
        }

        _ShaderReflection( spv );

        // Create shader module
        VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        createInfo.pCode    = spv.data();
        createInfo.codeSize = spv.size() * sizeof(uint32_t);
        vkCreateShaderModule( g_vulkan.device, &createInfo, ALLOCATOR, &m_shaderModule );
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ShaderReflection( const ArrayList<uint32_t>& spv )
    {
        spirv_cross::Compiler comp( spv );
        auto resources = comp.get_shader_resources();

        // Uniform buffers
        for (auto& resource : resources.uniform_buffers)
        {
            const auto& type = comp.get_type( resource.type_id );

            StringID    name        = SID( resource.name.c_str() );
            U32         setNum      = comp.get_decoration( resource.id, spv::DecorationDescriptorSet );
            U32         bindingNum  = comp.get_decoration( resource.id, spv::DecorationBinding );
            U32         bufferSize  = (U32)comp.get_declared_struct_size( type );
            ShaderUniformBufferDeclaration ubo{ name, setNum, bindingNum, bufferSize };

            auto members = ParseUniformBuffer( comp, type, 0, "" );
            for (auto& uniform : members)
                ubo._AddUniformDecl( uniform );

            m_uniformBuffers.emplace_back( ubo );
        }

        // Check for shader/material ubo
        for (auto& ubo : m_uniformBuffers)
        {
            String lower = StringUtils::toLower( ubo.getName().toString() );
            if (lower.find( MATERIAL_NAME ) != String::npos)
                m_materialUBO = &ubo;
            if (lower.find( SHADER_NAME ) != String::npos)
                m_shaderUBO = &ubo;
        }

        // Image Sampler
        for (auto& resource : resources.sampled_images)
        {
            const auto& type = comp.get_type(resource.type_id);

            DataType dataType = DataType::Unknown;
            switch (type.image.dim)
            {
            case spv::Dim::Dim1D:   dataType = DataType::Texture1D; break;
            case spv::Dim::Dim2D:   dataType = DataType::Texture2D; break;
            case spv::Dim::Dim3D:   dataType = DataType::Texture3D; break;
            case spv::Dim::DimCube: dataType = DataType::TextureCubemap; break;
            default: LOG_WARN_RENDERING( "VkShaderBase: Unknown data-type for a sampled image." );
            }

            U32 setNum      = comp.get_decoration( resource.id, spv::DecorationDescriptorSet );
            U32 bindingNum  = comp.get_decoration( resource.id, spv::DecorationBinding );
            ShaderResourceDeclaration res{ m_shaderType, setNum, bindingNum, SID( resource.name.c_str() ), dataType };
            m_resourceDeclarations.emplace_back( res );
        }

        // Push-Constant
        for (auto& resource : resources.push_constant_buffers)
        {
            const auto& type = comp.get_type( resource.type_id );

            auto members = ParseUniformBuffer( comp, type, 0, "" );

            m_pushConstant.sizeInBytes = (U32)comp.get_declared_struct_size( type );
            m_pushConstant.members = members;
        }
    }

    //----------------------------------------------------------------------
    ArrayList<uint32_t> CompileGLSLToSPV( const String& source, ShaderType shaderType, bool debug )
    {
        String shaderName;
        shaderc_shader_kind shaderKind = shaderc_glsl_infer_from_source;
        switch (shaderType)
        {
        case ShaderType::Vertex:    shaderKind = shaderc_vertex_shader;     shaderName = "vertex"; break;
        case ShaderType::Fragment:  shaderKind = shaderc_fragment_shader;   shaderName = "fragment"; break;
        case ShaderType::Geometry:  shaderKind = shaderc_geometry_shader;   shaderName = "geometry"; break;
        }

        shaderc::CompileOptions options;
        if (debug)
        {
            options.SetWarningsAsErrors();
            options.SetOptimizationLevel( shaderc_optimization_level_zero );
        }
        else
        {
            options.SetOptimizationLevel( shaderc_optimization_level_performance );
        }

        shaderc::Compiler compiler;
        auto result = compiler.CompileGlslToSpv( source, shaderKind, shaderName.c_str(), options );

        auto compileStatus = result.GetCompilationStatus();
        if (compileStatus != shaderc_compilation_status_success)
            throw std::runtime_error( "Failed to compile " + shaderName + " shader from source:\n" + result.GetErrorMessage() );

        return { result.begin(), result.end() };
    }

    //----------------------------------------------------------------------
    DataType GetDataTypeOfMember( const spirv_cross::SPIRType& memberType )
    {
        // vecsize equal to ROW
        switch (memberType.vecsize)
        {
        case 1:
            switch (memberType.basetype)
            {
            case spirv_cross::SPIRType::Float:   return DataType::Float;
            case spirv_cross::SPIRType::Double:  return DataType::Double;
            case spirv_cross::SPIRType::Int:     return DataType::Int;
            case spirv_cross::SPIRType::Boolean: return DataType::Boolean;
            case spirv_cross::SPIRType::Char:    return DataType::Char;
            }
            break;
        case 2:
            if (memberType.columns == 1)
                return DataType::Vec2;
        case 3:
            if (memberType.columns == 1)
                return DataType::Vec3;
        case 4:
            if (memberType.columns == 1)
                return DataType::Vec4;
            else if (memberType.columns == 4)
                return DataType::Matrix;
        }

        LOG_WARN_RENDERING( "VkShaderBase::getDataType(): Unrecognized Data-Type parsed." );
        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    ArrayList<ShaderUniformDeclaration> ParseUniformBuffer( const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& spirType, 
                                                            U32 parentOffset, const String& parentName )
    {
        ArrayList<ShaderUniformDeclaration> members;
        for (U32 i = 0; i < spirType.member_types.size(); i++)
        {
            // Get the member-name and add the "parent"-name infront of it e.g. "baseLight." + color
            auto memberName = parentName + comp.get_member_name( spirType.self, i );

            // Get the offset of the member and add the parent-offset to it
            U32 offset = parentOffset + comp.get_member_decoration( spirType.self, i, spv::DecorationOffset );

            const spirv_cross::SPIRType& memberType = comp.get_type( spirType.member_types[i] );
            if (memberType.basetype == spirv_cross::SPIRType::Struct) // Member is itself a struct, call recursively
            {
                auto structMembers = ParseUniformBuffer( comp, memberType, offset, memberName + "." );
                members.insert( members.end(), structMembers.begin(), structMembers.end() );
            }
            else // Member is not a struct. Get the size and type of it and push it to the vector.
            {
                U32 size = (U32)( comp.get_declared_struct_member_size( spirType, i ) );
                DataType dataType = GetDataTypeOfMember( memberType );
                members.emplace_back( SID( memberName.c_str() ), offset, size, dataType );
            }
        }
        return members;
    }

} } // End namespaces