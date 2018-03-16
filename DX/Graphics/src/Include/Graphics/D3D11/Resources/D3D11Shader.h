#pragma once
/**********************************************************************
    class: Shader (Shader.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_shader.hpp"
#include "../D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    class VertexShader;
    class PixelShader;

    //**********************************************************************
    class Shader : public IShader
    {
    public:
        Shader();
        ~Shader();

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void setShaderPaths(const OS::Path& vertPath, const OS::Path& fragPath) override;
        bool compile(CString entryPoint) override;
        ArrayList<OS::Path> recompile();
        bool isUpToDate() override;
        ArrayList<OS::Path> getShaderPaths() const override;

        void setRasterizationState(const RasterizationState& rzState) override;


    private:
        VertexShader*       m_pVertexShader   = nullptr;
        PixelShader*        m_pPixelShader    = nullptr;

        ID3D11DepthStencilState*    m_pDepthStencilState;
        ID3D11RasterizerState*      m_pRSState;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void bind() override;
        void drawMesh(IMesh* mesh, U32 subMeshIndex) override;

        //----------------------------------------------------------------------
        void _CreatePipeline();

        //----------------------------------------------------------------------
        Shader(const Shader& other)               = delete;
        Shader& operator = (const Shader& other)  = delete;
        Shader(Shader&& other)                    = delete;
        Shader& operator = (Shader&& other)       = delete;
    };

} } // End namespaces