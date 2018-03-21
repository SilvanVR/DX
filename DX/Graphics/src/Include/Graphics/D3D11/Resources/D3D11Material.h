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
        ~MaterialData() { SAFE_DELETE( m_pConstantBuffer ); }

        //----------------------------------------------------------------------
        //@Params:
        // "offset": Offset in bytes to put the data into.
        // "pData": Pointer to the actual data to copy from.
        //----------------------------------------------------------------------
        template <typename T>
        void push(U32 offset, T* pData)
        {
            ASSERT( ( offset + sizeof(T) ) < m_materialData.size() );
            memcpy( &m_materialData[offset], (void*)pData, sizeof(T) );
            m_gpuUpToDate = false;
        }

        //----------------------------------------------------------------------
        bool                    isUpToDate() const { return m_gpuUpToDate; }
        const ConstantBuffer*   getBuffer() const { return m_pConstantBuffer; }

        //----------------------------------------------------------------------
        void resize(U32 size) 
        { 
            SAFE_DELETE( m_pConstantBuffer );
            m_materialData.resize( size ); 
            m_pConstantBuffer = new D3D11::ConstantBuffer( size, BufferUsage::LONG_LIVED );
        }

        //----------------------------------------------------------------------
        void pushToGPU()
        {
            m_pConstantBuffer->update( m_materialData.data(), m_materialData.size() );
            m_gpuUpToDate = true;
        }

    private:
        ArrayList<Byte>         m_materialData;
        bool                    m_gpuUpToDate = true;
        D3D11::ConstantBuffer*  m_pConstantBuffer = nullptr;
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
        void _SetFloat(CString name, F32 val) override;
        void _SetVec4(CString name, const Math::Vec4& vec) override;

    private:
        // Contains the material data in a contiguous block of memory. Will be empty if not used for a shader.
        MaterialData m_materialDataVS;
        MaterialData m_materialDataPS;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void bind() override;
        void _ChangedShader() override;

        //----------------------------------------------------------------------
        void _CreateConstantBuffers();
        void _UpdateConstantBuffer();

        //----------------------------------------------------------------------
        Material(const Material& other)               = delete;
        Material& operator = (const Material& other)  = delete;
        Material(Material&& other)                    = delete;
        Material& operator = (Material&& other)       = delete;
    };

} } // End namespaces