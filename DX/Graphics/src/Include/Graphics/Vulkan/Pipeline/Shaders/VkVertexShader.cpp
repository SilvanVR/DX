#include "VkVertexShader.h"
/**********************************************************************
    class: VertexShader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void VertexShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        _CompileFromFile( path, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateShader( shaderBlob );
        } );
    }

    //----------------------------------------------------------------------
    void VertexShader::compileFromSource( const String& source, CString entryPoint )
    {
        _CompileFromSource( source, entryPoint, [this] (const ShaderBlob& shaderBlob) {
            _CreateShader( shaderBlob );
        } );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void VertexShader::_CreateShader( const ShaderBlob& shaderBlob )
    {
        // Create vertex shader
        VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        createInfo.pCode    = (U32*)shaderBlob.data;
        createInfo.codeSize = shaderBlob.size;

        vkCreateShaderModule( g_vulkan.device, &createInfo, ALLOCATOR, &m_shaderModule );
        //_CreateInputLayout( shaderBlob.data, shaderBlob.size );
    }

} } // End namespaces