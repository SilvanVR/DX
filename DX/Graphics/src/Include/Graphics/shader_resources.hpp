#pragma once
/**********************************************************************
    class: Several (shader_resources.hpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "enums.hpp"

namespace Graphics {

    //**********************************************************************
    class ShaderResourceDeclaration
    {
    public:
        ShaderResourceDeclaration(ShaderType shaderType, U32 bindingSlot, StringID name, DataType dataType)
            : m_shaderType( shaderType ), m_bindingSlot( bindingSlot ), m_name( name ), m_type( dataType ) {}

        inline ShaderType   getShaderType()     const { return m_shaderType; }
        inline U32          getBindingSlot()    const { return m_bindingSlot; }
        inline StringID     getName()           const { return m_name; }
        inline DataType     getDataType()       const { return m_type; }

    private:
        ShaderType  m_shaderType;
        U32         m_bindingSlot;
        StringID    m_name;
        DataType    m_type;
    };

    //**********************************************************************
    struct ShaderUniformDeclaration
    {
    public:
        ShaderUniformDeclaration(StringID name, U32 offset, U32 size, DataType dataType)
            : m_name( name ), m_offset( offset ), m_size( size ), m_type( dataType ) {}

        inline U32          getSize()       const { return m_size; }
        inline U32          getOffset()     const { return m_offset; }
        inline StringID     getName()       const { return m_name; }
        inline DataType     getDataType()   const { return m_type; }

        bool operator==(const ShaderUniformDeclaration& c) const { return m_name == c.m_name && m_offset == c.m_offset && m_size == c.m_size && m_type == c.m_type; }
        bool operator!=(const ShaderUniformDeclaration& c) const { return !(*this == c); }

    private:
        StringID    m_name;
        U32         m_offset;
        U32         m_size;
        DataType    m_type;
    };

    //**********************************************************************
    struct ShaderUniformBufferDeclaration
    {
    public:
        ShaderUniformBufferDeclaration(StringID name, U32 bindingSlot, U32 size)
            : m_name( name ), m_bindingSlot( bindingSlot ), m_size( size ) {}

        inline U32                                          getSize()           const { return m_size; }
        inline U32                                          getBindingSlot()    const { return m_bindingSlot; }
        inline StringID                                     getName()           const { return m_name; }
        inline const ArrayList<ShaderUniformDeclaration>&   getMembers()        const { return m_members; }

        inline const ShaderUniformDeclaration* getMember(StringID name) const 
        {
            for (auto& member : m_members)
                if (member.getName() == name)
                    return &member;
            return nullptr;
        }

        inline bool hasMember(StringID name) const
        {
            for (auto& member : m_members)
                if (member.getName() == name)
                    return true;
            return false;
        }

        // Used to check if two buffers are equal
        bool operator==(const ShaderUniformBufferDeclaration& c) const
        {
            if ((m_bindingSlot != c.m_bindingSlot) || (m_size != c.m_size)) return false;
            if (m_members.size() != c.m_members.size()) return false;
            for (I32 i = 0; i < m_members.size(); i++)
                if (m_members[i] != c.m_members[i])
                    return false;
            return true;
        }
        bool operator!=(const ShaderUniformBufferDeclaration& c) const { return !(*this == c); }

        void _AddUniformDecl(const ShaderUniformDeclaration& uniform) { m_members.push_back( uniform ); }

    private:
        StringID                            m_name;
        U32                                 m_bindingSlot;
        U32                                 m_size;
        ArrayList<ShaderUniformDeclaration> m_members;

    };

}