#include "D3D11ShaderCompiler.h"
/**********************************************************************
    class: ShaderCompiler (D3D11ShaderCompiler.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include <d3dcompiler.h>

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    ID3DBlob* ShaderCompiler::_Compile( String path, CString entryPoint, CString profile )
    {
        ID3DBlob* shaderBlob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    #ifdef _DEBUG
        flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
    #endif

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( path ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         entryPoint, profile, flags, 0, &shaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            if (errorBlob)
            {
                WARN_RENDERING( "Failed to compile Shader '" + path + "'." );
                WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
                SAFE_RELEASE( errorBlob );
            }
            else
            {
                WARN_RENDERING( "Missing shader-file: '" + path + "'.");
            }

            SAFE_RELEASE( shaderBlob );
        }

        return shaderBlob;
    }

} } // End namespaces