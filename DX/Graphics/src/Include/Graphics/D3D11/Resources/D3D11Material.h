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
    class MaterialData
    {
    public:
        //----------------------------------------------------------------------
        //@Params:
        // "offset": Offset in bytes to put the data into.
        // "pData": Pointer to the actual data to copy from.
        //----------------------------------------------------------------------
        template <typename T>
        void push(U32 offset, T& pData)
        {
            ASSERT( ( offset + sizeof(T) )  < m_materialData.size() );
            memcpy( &m_materialData[offset], (void*)&pData, sizeof(T) );
            m_upToDate = false;
        }

        //----------------------------------------------------------------------
        const Byte* data()          const { return m_materialData.data(); }
        U32         size()          const { return static_cast<U32>( m_materialData.size() ); }
        bool        isUpToDate()    const { return m_upToDate; }

        //----------------------------------------------------------------------
        void        resize(U32 size) { m_materialData.resize( size ); }
        void        setIsUpToDate() { m_upToDate = true; }

    private:
        ArrayList<Byte> m_materialData;
        bool            m_upToDate = true;
    };

    //**********************************************************************
    class Material : public IMaterial
    {
    public:
        Material() = default;
        ~Material() { _DestroyConstantBuffers(); }

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void setFloat(CString name, F32 val) override;
        void setVec4(CString name, const Math::Vec4& vec) override;

    private:
        D3D11::ConstantBuffer* m_pConstantBufferVS;
        D3D11::ConstantBuffer* m_pConstantBufferPS;

        // Contains the material data in a contiguous block of memory. Will be empty if not used for a shader.
        MaterialData        m_materialDataVS;
        MaterialData        m_materialDataPS;

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