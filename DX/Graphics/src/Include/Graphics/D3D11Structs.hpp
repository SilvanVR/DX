#pragma once
/**********************************************************************
    class: None (D3D11Structs.hpp)

    author: S. Hau
    date: May 12, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    // Contains infos about one specific member in a shader buffer
    struct ConstantBufferMemberInfo
    {
        StringID    name;
        U32         offset = 0;
        Size        size = 0;
        DataType    type = DataType::Unknown;

        bool operator==(const ConstantBufferMemberInfo& c) const { return name == c.name && offset == c.offset && size == c.size && type == c.type; }
        bool operator!=(const ConstantBufferMemberInfo& c) const { return !(*this == c); }
    };

    // Contains information about an buffer in a shader
    struct ConstantBufferInfo
    {
        U32                                 slot = 0;
        Size                                sizeInBytes = 0;
        ArrayList<ConstantBufferMemberInfo> members;

        bool operator==(const ConstantBufferInfo& c) const
        {
            if ((slot != c.slot) || (sizeInBytes != c.sizeInBytes)) return false;
            if (members.size() != c.members.size()) return false;
            for (I32 i = 0; i < members.size(); i++)
                if (members[i] != c.members[i])
                    return false;
            return true;
        }
        bool operator!=(const ConstantBufferInfo& c) const { return !(*this == c); }
    };

} } // End namespaces