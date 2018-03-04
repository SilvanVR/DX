#pragma once
/**********************************************************************
    class: Pass (D3D11Pass.h)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

#include "../D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Pass
    {
    public:
        Pass() {}
        ~Pass() {}

        //----------------------------------------------------------------------


    protected:


    private:
        ID3D11DepthStencilState*    pDepthStencilState;
        ID3D11RasterizerState*      pRSState;

        //----------------------------------------------------------------------
        Pass(const Pass& other)               = delete;
        Pass& operator = (const Pass& other)  = delete;
        Pass(Pass&& other)                    = delete;
        Pass& operator = (Pass&& other)       = delete;
    };


} } // End namespaces