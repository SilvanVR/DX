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
        // Constructor for dx11
        ShaderResourceDeclaration(ShaderType shaderType, U32 bindingSlot, StringID name, DataType dataType)
            : m_shaderType(shaderType), m_bindingSlot(bindingSlot), m_name(name), m_type(dataType) {}

        // Constructor for vulkan
        ShaderResourceDeclaration(ShaderType shaderType, U32 set, U32 binding, StringID name, DataType dataType)
            : m_shaderType(shaderType), m_bindingSet(set), m_bindingSlot(binding), m_name(name), m_type(dataType) {}

        inline ShaderType   getShaderType()     const { return m_shaderType; }
        inline U32          getBindingSet()     const { return m_bindingSet; }
        inline U32          getBindingSlot()    const { return m_bindingSlot; }
        inline StringID     getName()           const { return m_name; }
        inline DataType     getDataType()       const { return m_type; }

    private:
        ShaderType  m_shaderType;
        U32         m_bindingSlot;
        U32         m_bindingSet; // Not used in dx11
        StringID    m_name;
        DataType    m_type;
    };

    //**********************************************************************
    struct ShaderUniformDeclaration
    {
    public:
        ShaderUniformDeclaration(StringID name, U32 offset, U32 size, DataType dataType, U32 arraySize = 1)
            : m_name(name), m_offset(offset), m_sizeInBytes(size), m_type(dataType), m_arraySize(arraySize) {}

        inline U32          getSize()       const { return m_sizeInBytes; }
        inline U32          getOffset()     const { return m_offset; }
        inline StringID     getName()       const { return m_name; }
        inline DataType     getDataType()   const { return m_type; }

        bool operator==(const ShaderUniformDeclaration& c) const { return m_name == c.m_name && m_offset == c.m_offset && m_sizeInBytes == c.m_sizeInBytes && m_type == c.m_type && m_arraySize == c.m_arraySize; }
        bool operator!=(const ShaderUniformDeclaration& c) const { return !(*this == c); }

    private:
        StringID    m_name;
        U32         m_offset;
        U32         m_sizeInBytes;
        DataType    m_type;
        U32         m_arraySize;
    };

    //**********************************************************************
    struct ShaderUniformBufferDeclaration
    {
    public:
        // Constructor for dx11
        ShaderUniformBufferDeclaration(StringID name, U32 bindingSlot, U32 sizeInBytes)
            : m_name(name), m_bindingSlot(bindingSlot), m_sizeInBytes(sizeInBytes) {}

        // Constructor for vulkan
        ShaderUniformBufferDeclaration(StringID name, U32 set, U32 binding, U32 sizeInBytes)
            : m_name(name), m_bindingSet(set), m_bindingSlot(binding), m_sizeInBytes(sizeInBytes) {}

        inline U32                                          getSize()           const { return m_sizeInBytes; }
        inline U32                                          getBindingSlot()    const { return m_bindingSlot; }
        inline U32                                          getBindingSet()     const { return m_bindingSet; }
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
            if ((m_bindingSlot != c.m_bindingSlot) || (m_sizeInBytes != c.m_sizeInBytes)) return false;
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
        U32                                 m_bindingSet; // Not used in dx11
        U32                                 m_sizeInBytes;
        ArrayList<ShaderUniformDeclaration> m_members;
    };

}