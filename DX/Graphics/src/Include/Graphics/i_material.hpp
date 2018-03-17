#pragma once
/**********************************************************************
    class: IMaterial (i_material.hpp)

    author: S. Hau
    date: March 12, 2018

    Interface for a Material class. Each material is rendered by a
    shader (which can consists of vertex/frag/geo/tesselation) and
    contains the necessary data for this shader.
**********************************************************************/

#include "i_shader.hpp"

namespace Graphics {

    //**********************************************************************
    class MaterialData
    {
    public:

        template <typename T>
        void push(U32 offset, T& pData)
        {
            ASSERT( (offset + sizeof(T)) < m_materialData.size() );
            memcpy( &m_materialData[offset], (void*)&pData, sizeof(T) );
            m_upToDate = false;
        }

        bool isUpToDate() const { return m_upToDate; }

        const Byte* data() const { return m_materialData.data(); }
        U32         size() const { return static_cast<U32>( m_materialData.size() ); }
        void        resize(U32 size) { m_materialData.resize( size ); }

        void setIsUpToDate(){ m_upToDate = true; }

    private:
        ArrayList<Byte> m_materialData;
        bool            m_upToDate = false;
    };
    
    //**********************************************************************
    class IMaterial
    {
    public:
        IMaterial() = default;
        virtual ~IMaterial() {}

        //----------------------------------------------------------------------
        void        setShader(IShader* shader) { m_shader = shader; _ChangedShader(); }

        //----------------------------------------------------------------------
        IShader*    getShader() const { return m_shader; }

        //----------------------------------------------------------------------
        void setFloat(CString name, F32 val)
        {
            //@Get offset from shader for this name
            //@TODO: Updating depending on shader, e.g. can be vertex or fragment
            U32 offset = 16;
            m_materialDataVS.push( offset, val );
        }

        //----------------------------------------------------------------------
        void setVec4(CString name, const Math::Vec4& vec)
        {
            //@Get offset from shader for this name
            U32 offset = 0;
            m_materialDataVS.push( offset, vec );
        }

    protected:
        IShader*            m_shader = nullptr;

        // Contains the material data in a contiguous block of memory
        MaterialData        m_materialDataVS;

        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        virtual void bind() = 0;
        virtual void _ChangedShader() = 0;

    private:
        //----------------------------------------------------------------------
        IMaterial(const IMaterial& other)               = delete;
        IMaterial& operator = (const IMaterial& other)  = delete;
        IMaterial(IMaterial&& other)                    = delete;
        IMaterial& operator = (IMaterial&& other)       = delete;
    };

    using Material = IMaterial;

} // End namespaces

//**********************************************************************