#pragma once
/**********************************************************************
    class: Material (D3D11Material.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_material.h"
#include "../Pipeline/Buffers/D3D11MappedConstantBuffer.h"

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
        void _SetInt(StringID name, I32 val)                            override { _UpdateConstantBuffer(name, &val); }
        void _SetFloat(StringID name, F32 val)                          override { _UpdateConstantBuffer(name, &val); }
        void _SetVec4(StringID name, const Math::Vec4& vec)             override { _UpdateConstantBuffer(name, &vec); }
        void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) override { _UpdateConstantBuffer(name, &matrix); }
        void _SetTexture(StringID name, const TexturePtr& texture)      override {}

    private:
        // Contains the material data in a contiguous block of memory. Will be empty if not used for a shader.
        MappedConstantBuffer* m_materialDataVS = nullptr;
        MappedConstantBuffer* m_materialDataPS = nullptr;

        //// Stores texture as a d3d11 texture and the bindslot
        //struct TextureCache
        //{
        //    U32                         bindSlot;
        //    ShaderType                  shaderType;
        //    D3D11::IBindableTexture*    texture;
        //};
        //ArrayList<TexturePtr> m_textureCache;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void bind() override;
        void _ChangedShader() override;

        //----------------------------------------------------------------------
        void _DestroyConstantBuffers();
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer(StringID name, const void* pData);

        //----------------------------------------------------------------------
        Material(const Material& other)               = delete;
        Material& operator = (const Material& other)  = delete;
        Material(Material&& other)                    = delete;
        Material& operator = (Material&& other)       = delete;
    };

} } // End namespaces