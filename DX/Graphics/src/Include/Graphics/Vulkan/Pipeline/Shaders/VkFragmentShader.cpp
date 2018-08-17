#include "VkFragmentShader.h"
/**********************************************************************
    class: FragmentShader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "OS/FileSystem/file_system.h"
#include "OS/FileSystem/file.h"

namespace Graphics { namespace Vulkan {
    
    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void FragmentShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        _CompileFromFile( path, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateShader(shaderBlob);
        } );
    }

    //----------------------------------------------------------------------
    void FragmentShader::compileFromSource( const String& source, CString entryPoint )
    {
        _CompileFromSource( source, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateShader( shaderBlob );
        } );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void FragmentShader::_CreateShader( const ShaderBlob& shaderBlob )
    {

    }


} } // End namespaces