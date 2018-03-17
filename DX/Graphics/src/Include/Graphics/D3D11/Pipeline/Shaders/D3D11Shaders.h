#pragma once
/**********************************************************************
    class: ShaderBase + VertexShader etc. (D3D11Shaders.h)

    author: S. Hau
    date: December 3, 2017
**********************************************************************/

#include "../../D3D11.hpp"
#include "OS/FileSystem/path.h"
#include "vertex_layout.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase( const OS::Path& path );
        virtual ~ShaderBase() = 0;

        //----------------------------------------------------------------------
        const OS::Path& getFilePath() const { return m_filePath; }
        ID3DBlob*       getShaderBlob() { return m_shaderBaseBlob; }
        CString         getEntryPoint() const { return m_entryPoint.c_str(); }

        virtual void bind() = 0;
        virtual bool compile(CString entryPoint) = 0;

        bool isUpToDate();

    protected:
        ID3DBlob*       m_shaderBaseBlob = nullptr;
        String          m_entryPoint = "main";
        OS::Path        m_filePath;
        OS::SystemTime  m_fileTimeAtCompilation;

        //----------------------------------------------------------------------
        bool _Compile( CString entryPoint, CString profile );

    private:
        //----------------------------------------------------------------------
        ShaderBase(const ShaderBase& other)               = delete;
        ShaderBase& operator = (const ShaderBase& other)  = delete;
        ShaderBase(ShaderBase&& other)                    = delete;
        ShaderBase& operator = (ShaderBase&& other)       = delete;
    };

    //**********************************************************************
    class VertexShader : public ShaderBase
    {
    public:
        VertexShader(CString path);
        ~VertexShader();

        void bind() override;
        bool compile(CString entryPoint) override;

        const VertexLayout& getVertexLayout() const { return m_vertexLayout; }

    private:
        ID3D11VertexShader* m_pVertexShader = nullptr;
        ID3D11InputLayout*  m_pInputLayout  = nullptr;

        VertexLayout        m_vertexLayout;

        //----------------------------------------------------------------------
        void _CreateInputLayout(ID3DBlob* pShaderBlob);
        void _AddToVertexLayout(String semanticName);


        //----------------------------------------------------------------------
        VertexShader(const VertexShader& other)               = delete;
        VertexShader& operator = (const VertexShader& other)  = delete;
        VertexShader(VertexShader&& other)                    = delete;
        VertexShader& operator = (VertexShader&& other)       = delete;
    };

    //**********************************************************************
    class PixelShader : public ShaderBase
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


    // Get the latest profile for the specified ShaderBase type.
    template< class ShaderBaseClass >
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