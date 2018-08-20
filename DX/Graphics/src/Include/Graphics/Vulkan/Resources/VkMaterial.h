#pragma once
/**********************************************************************
    class: Material

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_material.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Material : public IMaterial
    {
    public:
        Material() = default;
        ~Material() { _DestroyConstantBuffers(); }

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void _SetInt(StringID name, I32 val)                            override { _UpdateConstantBuffer(name, &val); }
        void _SetFloat(StringID name, F32 val)                          override { _UpdateConstantBuffer(name, &val); }
        void _SetVec4(StringID name, const Math::Vec4& vec)             override { _UpdateConstantBuffer(name, &vec); }
        void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) override { _UpdateConstantBuffer(name, &matrix); }
        void _SetTexture(StringID name, const TexturePtr& texture)      override {}

    private:
        // Contains the material data in a contiguous block of memory. Will be empty if not used for a shader.
        //MappedConstantBuffer* m_materialDataVS = nullptr;
        //MappedConstantBuffer* m_materialDataPS = nullptr;
        //MappedConstantBuffer* m_materialDataGS = nullptr;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void bind() override;
        void _ChangedShader() override;

        //----------------------------------------------------------------------
        void _DestroyConstantBuffers();
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer(StringID name, const void* pData);

        NULL_COPY_AND_ASSIGN(Material)
    };

} } // End namespaces