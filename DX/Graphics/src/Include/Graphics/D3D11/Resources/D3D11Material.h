#pragma once
/**********************************************************************
    class: Material (D3D11Material.h)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

#include "../../i_material.h"
#include "../Pipeline/Buffers/D3D11Buffers.h"
#include "D3D11IBindableTexture.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // Stores material data in an contiguous block of memory on the cpu.
    // If it gets updated, the data will be flushed to the gpu on the next bind().
    //**********************************************************************
    class MaterialData
    {
    public:
        ~MaterialData() { SAFE_DELETE( m_pConstantBuffer ); }

        //----------------------------------------------------------------------
        bool    isUpToDate()    const { return m_gpuUpToDate; }
        bool    hasBuffer()     const { return m_pConstantBuffer != nullptr; }

        //----------------------------------------------------------------------
        //@Params:
        // "offset": Offset in bytes to put the data into.
        // "pData": Pointer to the actual data to copy from.
        //----------------------------------------------------------------------
        void push(U32 offset, const void* pData, Size sizeInBytes);

        //----------------------------------------------------------------------
        // Resize the buffer on cpu & gpu.
        // @Params:
        // "size": New size in bytes of the buffers.
        // "bindSlot": Bind-Slot for the constant buffer.
        //----------------------------------------------------------------------
        void resize(U32 size, U32 bindSlot);

        //----------------------------------------------------------------------
        // Bind the constant buffer to the device context. Might update it before.
        // @Params:
        // "shaderType": The shader to bind the constant buffer.
        //----------------------------------------------------------------------
        void bind(ShaderType shaderType);

    private:
        ArrayList<Byte>         m_materialData;
        D3D11::ConstantBuffer*  m_pConstantBuffer   = nullptr;
        U32                     m_bindSlot          = 0;
        bool                    m_gpuUpToDate       = true;
    };

    //**********************************************************************
    class Material : public IMaterial
    {
    public:
        Material() = default;
        ~Material() = default;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        bool _SetInt(StringID name, I32 val)                            override { return _UpdateConstantBuffer( name, &val, sizeof( val ) ); }
        bool _SetFloat(StringID name, F32 val)                          override { return _UpdateConstantBuffer( name, &val, sizeof( val ) ); }
        bool _SetVec4(StringID name, const Math::Vec4& vec)             override { return _UpdateConstantBuffer( name, &vec, sizeof( vec ) ); }
        bool _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) override { return _UpdateConstantBuffer( name, &matrix, sizeof( matrix ) ); }
        bool _SetTexture(StringID name, TexturePtr texture) override;

    private:
        // Contains the material data in a contiguous block of memory. Will be empty if not used for a shader.
        MaterialData m_materialDataVS;
        MaterialData m_materialDataPS;

        // Stores texture as a d3d11 texture and the bindslot
        struct TextureCache
        {
            U32                         bindSlot;
            D3D11::IBindableTexture*    texture;
        };
        ArrayList<TextureCache> m_textureCache;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void bind() override;
        void _ChangedShader() override;

        //----------------------------------------------------------------------
        void _CreateConstantBuffers();
        bool _UpdateConstantBuffer(StringID name, const void* pData, Size sizeInBytes);

        //----------------------------------------------------------------------
        Material(const Material& other)               = delete;
        Material& operator = (const Material& other)  = delete;
        Material(Material&& other)                    = delete;
        Material& operator = (Material&& other)       = delete;
    };

} } // End namespaces