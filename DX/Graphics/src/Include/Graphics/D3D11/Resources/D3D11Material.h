#pragma once
/**********************************************************************
    class: Material (D3D11Material.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_material.hpp"
#include "../Pipeline/Buffers/D3D11Buffers.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Material : public IMaterial
    {
    public:
        Material() = default;
        ~Material() { _DestroyConstantBuffers(); }

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------

    private:
        D3D11::ConstantBuffer* m_pConstantBufferVS;
        D3D11::ConstantBuffer* m_pConstantBufferPS;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void bind() override;
        void _ChangedShader() override;

        //----------------------------------------------------------------------
        void _DestroyConstantBuffers();
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer();

        //----------------------------------------------------------------------
        Material(const Material& other)               = delete;
        Material& operator = (const Material& other)  = delete;
        Material(Material&& other)                    = delete;
        Material& operator = (Material&& other)       = delete;
    };

} } // End namespaces