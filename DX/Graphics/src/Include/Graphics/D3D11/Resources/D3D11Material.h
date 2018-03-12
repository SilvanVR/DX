#pragma once
/**********************************************************************
    class: D3D11Material (D3D11Material.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_material.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class D3D11Material : public IMaterial
    {
    public:
        D3D11Material() = default;
        ~D3D11Material() = default;

        //----------------------------------------------------------------------
    protected:


    private:
        //----------------------------------------------------------------------
        D3D11Material(const D3D11Material& other)               = delete;
        D3D11Material& operator = (const D3D11Material& other)  = delete;
        D3D11Material(D3D11Material&& other)                    = delete;
        D3D11Material& operator = (D3D11Material&& other)       = delete;
    };

} } // End namespaces