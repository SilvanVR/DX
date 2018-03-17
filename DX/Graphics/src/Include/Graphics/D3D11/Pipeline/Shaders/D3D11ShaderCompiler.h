#pragma once
/**********************************************************************
    class: ShaderCompiler (D3D11ShaderCompiler.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "../../D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class ShaderCompiler
    {
        ShaderCompiler() = default;
        ~ShaderCompiler() = default;
    public:

        //----------------------------------------------------------------------
        template <class T>
        static ID3DBlob* Compile(String path, CString entryPoint)
        {
            return _Compile( path, entryPoint, GetLatestProfile<T>().c_str() );
        }


    private:
        static ID3DBlob* _Compile(String path, CString entryPoint, CString profile);

        //----------------------------------------------------------------------
        ShaderCompiler(const ShaderCompiler& other)               = delete;
        ShaderCompiler& operator = (const ShaderCompiler& other)  = delete;
        ShaderCompiler(ShaderCompiler&& other)                    = delete;
        ShaderCompiler& operator = (ShaderCompiler&& other)       = delete;
    };

    // Get the latest profile for the specified Shader type.
    template<class ShaderCompilerClass>
    String GetLatestProfile();

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11VertexShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "vs_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "vs_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "vs_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "vs_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "vs_4_0_level_9_1";
        }
        return "";
    }

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11PixelShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "ps_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "ps_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "ps_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "ps_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "ps_4_0_level_9_1";
        }
        return "";
    }

} } // End namespaces