#pragma once
/**********************************************************************
    class: IShader + VertexShader etc. (D3D11IShader.h)

    author: S. Hau
    date: December 3, 2017
**********************************************************************/

#include "../../D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class IShader
    {
    public:
        IShader(CString path);
        virtual ~IShader() = 0;

        //----------------------------------------------------------------------
        const String&   getFilePath() const { return m_filePath; }
        ID3DBlob*       getShaderBlob() { return m_IShaderBlob; }

        virtual void bind() = 0;
        virtual bool compile(CString entryPoint) = 0;

    protected:
        ID3DBlob*   m_IShaderBlob = nullptr;
        String      m_filePath;

        //----------------------------------------------------------------------
        bool _Compile( CString entryPoint, CString profile );

    private:
        //----------------------------------------------------------------------
        IShader(const IShader& other)               = delete;
        IShader& operator = (const IShader& other)  = delete;
        IShader(IShader&& other)                    = delete;
        IShader& operator = (IShader&& other)       = delete;
    };

    //**********************************************************************
    class VertexShader : public IShader
    {
    public:
        VertexShader(CString path);
        ~VertexShader();

        void bind() override;
        bool compile(CString entryPoint) override;

    private:
        ID3D11VertexShader* m_pVertexShader = nullptr;

        //----------------------------------------------------------------------
        VertexShader(const VertexShader& other)               = delete;
        VertexShader& operator = (const VertexShader& other)  = delete;
        VertexShader(VertexShader&& other)                    = delete;
        VertexShader& operator = (VertexShader&& other)       = delete;
    };

    //**********************************************************************
    class PixelShader : public IShader
    {
    public:
        PixelShader(CString path);
        ~PixelShader();

        void bind() override;
        bool compile(CString entryPoint) override;

    private:
        ID3D11PixelShader* m_pPixelShader = nullptr;

        //----------------------------------------------------------------------
        PixelShader(const PixelShader& other)               = delete;
        PixelShader& operator = (const PixelShader& other)  = delete;
        PixelShader(PixelShader&& other)                    = delete;
        PixelShader& operator = (PixelShader&& other)       = delete;
    };


    //// Get the latest profile for the specified IShader type.
    //template< class IShaderClass >
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

} } // End namespaces