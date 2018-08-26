#include "VkVertexShader.h"
/**********************************************************************
    class: VertexShader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    // Input ending with this are treated as instance attributes
    #define SEMANTIC_INSTANCED "_INSTANCE"

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void VertexShader::_ShaderReflection( const ArrayList<uint32_t>& spv )
    {
        m_vertexLayout.clear();

        // Reflect input layout
        //spirv_cross::Compiler comp( spv );
        //for (const auto& input : comp.get_shader_resources().stage_inputs)
        //{
        //    const auto& type = comp.get_type(input.type_id);
        //    U32 location = comp.get_decoration( input.id, spv::Decoration::DecorationLocation );
        //    U32 binding = comp.get_decoration( input.id, spv::Decoration::DecorationBinding );

        //    String name = input.name;
        //    Size pos = name.find( SEMANTIC_INSTANCED );
        //    constexpr Size sizeOfInstancedName = (sizeof(SEMANTIC_INSTANCED) / sizeof(char)) - 1;
        //    Size posIfNameIsAtEnd = name.size() - sizeOfInstancedName;
        //    bool instanced = (pos != String::npos) && (pos == posIfNameIsAtEnd);

        //    if (instanced)  // Cut-off the "SEMANTIC_INSTANCED"
        //        name = name.substr( 0, pos );

        //    auto typeInfo = GetTypeInfo( type );
        //    if (typeInfo.format == VK_FORMAT_UNDEFINED)
        //        LOG_ERROR_RENDERING( "VertexShader::_CreateVkInputLayout: Unknown format." );

        //    VkVertexInputAttributeDescription attrDesc{};
        //    attrDesc.binding    = binding;
        //    attrDesc.location   = location;
        //    attrDesc.format     = typeInfo.format;
        //    m_attrDesc.push_back( attrDesc );

        //    VkVertexInputBindingDescription bindingDesc{};
        //    bindingDesc.inputRate = instanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        //    bindingDesc.binding   = binding;
        //    bindingDesc.stride    = typeInfo.sizeInBytes;
        //    m_bindingDesc.push_back( bindingDesc );

        //    m_vertexLayout.add( { SID( name.c_str() ), typeInfo.sizeInBytes, instanced } );
        //}

        // Reflect all other resources
        ShaderBase::_ShaderReflection( spv );
    }

} } // End namespaces