#pragma once
/**********************************************************************
    class: D3D11Shader (D3D11Shader.h)

    author: S. Hau
    date: December 3, 2017

**********************************************************************/

#include "D3D11.hpp"

namespace Core { namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Shader
    {
    public:
        Shader(CString path);
        virtual ~Shader() = 0;

        //----------------------------------------------------------------------
        const String& getFilePath() const { return m_filePath; }
        ID3DBlob* getShaderBlob() { return m_shaderBlob; }

        virtual bool compile(const char* entryPoint) = 0;
        virtual void bind() = 0;

    protected:
        ID3DBlob*   m_shaderBlob = nullptr;
        String      m_filePath;

        //----------------------------------------------------------------------
        bool _Compile( CString entryPoint, CString profile );

    private:
        //----------------------------------------------------------------------
        Shader(const Shader& other)               = delete;
        Shader& operator = (const Shader& other)  = delete;
        Shader(Shader&& other)                    = delete;
        Shader& operator = (Shader&& other)       = delete;
    };

    //**********************************************************************
    class VertexShader : public Shader
    {
    public:
        VertexShader(CString path);
        ~VertexShader();

        bool compile(CString entryPoint) override;
        void bind() override { g_pImmediateContext->VSSetShader( m_pVertexShader, NULL, 0 ); };

    private:
        ID3D11VertexShader* m_pVertexShader = nullptr;

        //----------------------------------------------------------------------
        VertexShader(const VertexShader& other)               = delete;
        VertexShader& operator = (const VertexShader& other)  = delete;
        VertexShader(VertexShader&& other)                    = delete;
        VertexShader& operator = (VertexShader&& other)       = delete;
    };

    //**********************************************************************
    class PixelShader : public Shader
    {
    public:
        PixelShader(CString path);
        ~PixelShader();

        virtual bool compile(CString entryPoint) override;
        void bind() override { g_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 ); };

    private:
        ID3D11PixelShader* m_pPixelShader = nullptr;

        //----------------------------------------------------------------------
        PixelShader(const PixelShader& other)               = delete;
        PixelShader& operator = (const PixelShader& other)  = delete;
        PixelShader(PixelShader&& other)                    = delete;
        PixelShader& operator = (PixelShader&& other)       = delete;
    };


    //// Get the latest profile for the specified shader type.
    //template< class ShaderClass >
    //String GetLatestProfile();

    //template<>
    //String GetLatestProfile<ID3D11VertexShader>()
    //{
    //    // Query the current feature level:
    //    D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();

    //    switch (featureLevel)
    //    {
    //    case D3D_FEATURE_LEVEL_11_1:
    //    case D3D_FEATURE_LEVEL_11_0:
    //    {
    //        return "vs_5_0";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_10_1:
    //    {
    //        return "vs_4_1";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_10_0:
    //    {
    //        return "vs_4_0";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_9_3:
    //    {
    //        return "vs_4_0_level_9_3";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_9_2:
    //    case D3D_FEATURE_LEVEL_9_1:
    //    {
    //        return "vs_4_0_level_9_1";
    //    }
    //    break;
    //    }

    //    return "";
    //}

    //template<>
    //String GetLatestProfile<ID3D11PixelShader>()
    //{
    //    // Query the current feature level:
    //    D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
    //    switch (featureLevel)
    //    {
    //    case D3D_FEATURE_LEVEL_11_1:
    //    case D3D_FEATURE_LEVEL_11_0:
    //    {
    //        return "ps_5_0";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_10_1:
    //    {
    //        return "ps_4_1";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_10_0:
    //    {
    //        return "ps_4_0";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_9_3:
    //    {
    //        return "ps_4_0_level_9_3";
    //    }
    //    break;
    //    case D3D_FEATURE_LEVEL_9_2:
    //    case D3D_FEATURE_LEVEL_9_1:
    //    {
    //        return "ps_4_0_level_9_1";
    //    }
    //    break;
    //    }
    //    return "";
    //}

} } } // End namespaces