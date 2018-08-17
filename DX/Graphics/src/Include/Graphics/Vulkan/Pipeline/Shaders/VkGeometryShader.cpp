#include "VkGeometryShader.h"
/**********************************************************************
    class: GeometryShader

    author: S. Hau
    date: August 18, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void GeometryShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        _CompileFromFile( path, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateShader( shaderBlob );
        } );
    }

    //----------------------------------------------------------------------
    void GeometryShader::compileFromSource( const String& source, CString entryPoint )
    {
        _CompileFromSource( source, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateShader( shaderBlob );
        });
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void GeometryShader::_CreateShader( const ShaderBlob& shaderBlob )
    {

    }

} } // End namespaces