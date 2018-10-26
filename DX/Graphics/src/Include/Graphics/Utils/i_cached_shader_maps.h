#pragma once
/**********************************************************************
    class: ICachedShaderMaps

    author: S. Hau
    date: October 26, 2018

    Interface for uploading data to a shader (constant/uniform buffer).
    Data is also cached in the RAM for retrieval if desired.
**********************************************************************/

#include "forward_declarations.hpp"

namespace Graphics {

    //**********************************************************************
    class ICachedShaderMaps
    {
    public:
        ICachedShaderMaps() = default;
        virtual ~ICachedShaderMaps() {}

        //----------------------------------------------------------------------
        I32                 getInt(StringID name)       const;
        F32                 getFloat(StringID name)     const;
        Math::Vec4          getVec4(StringID name)      const;
        DirectX::XMMATRIX   getMatrix(StringID name)    const;
        Color               getColor(StringID name)     const;
        TexturePtr          getTexture(StringID name)   const;

        //----------------------------------------------------------------------
        I32                 getInt(CString name)       const { return getInt(SID(name)); }
        F32                 getFloat(CString name)     const { return getFloat(SID(name)); }
        Math::Vec4          getVec4(CString name)      const { return getVec4(SID(name)); }
        DirectX::XMMATRIX   getMatrix(CString name)    const { return getMatrix(SID(name)); }
        Color               getColor(CString name)     const { return getColor(SID(name)); }
        TexturePtr          getTexture(CString name)   const { return getTexture(SID(name)); }

        //----------------------------------------------------------------------
        void setInt(StringID name, I32 val);
        void setFloat(StringID name, F32 val);
        void setVec4(StringID name, const Math::Vec4& vec);
        void setMatrix(StringID name, const DirectX::XMMATRIX& matrix);
        void setColor(StringID name, Color color);
        void setTexture(StringID name, const TexturePtr& tex);
        void setData(StringID name, const void* data) { _SetData(name, data); }

        void setInt(CString name, I32 val)                           { setInt(SID(name), val); }
        void setFloat(CString name, F32 val)                         { setFloat(SID(name), val); }
        void setVec4(CString name, const Math::Vec4& vec)            { setVec4(SID(name), vec); }
        void setMatrix(CString name, const DirectX::XMMATRIX& matrix){ setMatrix(SID(name), matrix); }
        void setColor(CString name, Color color)                     { setColor(SID(name), color); }
        void setTexture(CString name, const TexturePtr& tex)         { setTexture(SID(name), tex); }
        void setData(CString name, const void* data)                 { setData(SID(name), data); }

        //----------------------------------------------------------------------
        bool hasInt(StringID name)       const { return m_intMap.find( name ) != m_intMap.end(); }
        bool hasFloat(StringID name)     const { return m_floatMap.find( name ) != m_floatMap.end(); }
        bool hasVec4(StringID name)      const { return m_vec4Map.find( name ) != m_vec4Map.end(); }
        bool hasMatrix(StringID name)    const { return m_matrixMap.find( name ) != m_matrixMap.end(); }
        bool hasTexture(StringID name)   const { return m_textureMap.find( name ) != m_textureMap.end(); }

        bool hasInt(CString name)       const { return hasInt(SID(name)); }
        bool hasFloat(CString name)     const { return hasFloat(SID(name)); }
        bool hasVec4(CString name)      const { return hasVec4(SID(name)); }
        bool hasMatrix(CString name)    const { return hasMatrix(SID(name)); }
        bool hasTexture(CString name)   const { return hasTexture(SID(name)); }

    protected:
        // Data maps
        HashMap<StringID, I32>                          m_intMap;
        HashMap<StringID, F32>                          m_floatMap;
        HashMap<StringID, Math::Vec4>                   m_vec4Map;
        HashMap<StringID, DirectX::XMMATRIX>            m_matrixMap;
        HashMap<StringID, TexturePtr>                   m_textureMap;

        //----------------------------------------------------------------------
        // Clears all data in all data maps.
        //----------------------------------------------------------------------
        void _ClearAllMaps();

        // Each API should decide themselves how to efficiently update their data
        virtual void _SetInt(StringID name, I32 val) = 0;
        virtual void _SetFloat(StringID name, F32 val) = 0;
        virtual void _SetVec4(StringID name, const Math::Vec4& vec) = 0;
        virtual void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) = 0;
        virtual void _SetData(StringID name, const void* data) = 0;

        virtual void _WarnMissingInt(StringID name) const = 0;
        virtual void _WarnMissingFloat(StringID name) const = 0;
        virtual void _WarnMissingColor(StringID name) const = 0;
        virtual void _WarnMissingVec4(StringID name) const = 0;
        virtual void _WarnMissingMatrix(StringID name) const = 0;
        virtual void _WarnMissingTexture(StringID name) const = 0;

        virtual bool _HasShaderInt(StringID name) const = 0;
        virtual bool _HasShaderFloat(StringID name) const = 0;
        virtual bool _HasShaderColor(StringID name) const = 0;
        virtual bool _HasShaderVec4(StringID name) const = 0;
        virtual bool _HasShaderMatrix(StringID name) const = 0;
        virtual bool _HasShaderTexture(StringID name) const = 0;

        NULL_COPY_AND_ASSIGN(ICachedShaderMaps)
    };

} // End namespaces