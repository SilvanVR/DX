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
        bool                compile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) override;
        bool                compile(const String& vertSrc, const String& fragSrc, CString entryPoint) override;
        ArrayList<OS::Path> recompile();
        bool                isUpToDate() override;
        ArrayList<OS::Path> getShaderPaths() const override;
        const VertexLayout& getVertexLayout() const override;

        void setRasterizationState(const RasterizationState& rzState) override;
        void setDepthStencilState(const DepthStencilState& dsState) override;
        void setBlendState(const BlendState& bState) override;

        //----------------------------------------------------------------------
        const VertexShader* getVertexShader() const { return m_pVertexShader.get(); }
        const PixelShader*  getPixelShader() const { return m_pPixelShader.get(); }

    private:
        std::unique_ptr<VertexShader>   m_pVertexShader = nullptr;
        std::unique_ptr<PixelShader>    m_pPixelShader  = nullptr;

        ID3D11DepthStencilState*        m_pDepthStencilState;
        ID3D11RasterizerState*          m_pRSState;
        ID3D11BlendState*               m_pBlendState;

        //----------------------------------------------------------------------
        // IShader Interface
        //----------------------------------------------------------------------
        void bind() override;

        //----------------------------------------------------------------------
        void _CreatePipeline();

        //----------------------------------------------------------------------
        Shader(const Shader& other)               = delete;
        Shader& operator = (const Shader& other)  = delete;
        Shader(Shader&& other)                    = delete;
        Shader& operator = (Shader&& other)       = delete;
    };

} } // End namespaces