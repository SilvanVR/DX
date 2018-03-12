#pragma once
/**********************************************************************
    class: Material (D3D11Material.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_material.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Material : public IMaterial
    {
    public:
        Material() = default;
        ~Material() = default;

        //----------------------------------------------------------------------

    private:
        //----------------------------------------------------------------------
        Material(const Material& other)               = delete;
        Material& operator = (const Material& other)  = delete;
        Material(Material&& other)                    = delete;
        Material& operator = (Material&& other)       = delete;
    };

} } // End namespaces