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
        Shader() = default;
        ~Shader();

        //----------------------------------------------------------------------
        void setShaderPaths(const OS::Path& vertPath, const OS::Path& fragPath) override;
        bool compile(CString entryPoint) override;
        bool isUpToDate() override;
        ArrayList<OS::Path> getShaderPaths() const override;

        //----------------------------------------------------------------------
        void bind() override;
        void drawMesh(IMesh* mesh, U32 subMeshIndex) override;


    private:
        VertexShader*       pVertexShader   = nullptr;
        PixelShader*        pPixelShader    = nullptr;

        //ID3D11DepthStencilState*    pDepthStencilState;
        //ID3D11RasterizerState*      pRSState;


        //----------------------------------------------------------------------
        Shader(const Shader& other)               = delete;
        Shader& operator = (const Shader& other)  = delete;
        Shader(Shader&& other)                    = delete;
        Shader& operator = (Shader&& other)       = delete;
    };

} } // End namespaces