#pragma once
/**********************************************************************
    class: IMaterial (i_material.h)

    author: S. Hau
    date: March 12, 2018

    Interface for a Material class. Each material is rendered by a
    shader (which can consists of vertex/frag/geo/tesselation) and
    contains the necessary data for this shader.
**********************************************************************/

#include "i_shader.hpp"
#include "i_texture.h"

namespace Graphics {

    //**********************************************************************
    class IMaterial
    {
    public:
        IMaterial() = default;
        virtual ~IMaterial() {}

        //----------------------------------------------------------------------
        ShaderPtr       getShader() const { return m_shader; }
        const String&   getName()   const { return m_name; }
        void            setShader(ShaderPtr shader);
        void            setName(const String& name) { m_name = name; }

        //**********************************************************************
        // MATERIAL PARAMETERS
        //**********************************************************************
        //----------------------------------------------------------------------
        I32                 getInt(StringID name)       const;
        F32                 getFloat(StringID name)     const;
        Math::Vec4          getVec4(StringID name)      const;
        DirectX::XMMATRIX   getMatrix(StringID name)    const;
        Color               getColor(StringID name)     const;
        TexturePtr          getTexture(StringID name)   const;
        DataType            getDataType(StringID name)  const;

        //----------------------------------------------------------------------
        I32                 getInt(CString name)       const { return getInt(SID(name)); }
        F32                 getFloat(CString name)     const { return getFloat(SID(name)); }
        Math::Vec4          getVec4(CString name)      const { return getVec4(SID(name)); }
        DirectX::XMMATRIX   getMatrix(CString name)    const { return getMatrix(SID(name)); }
        Color               getColor(CString name)     const { return getColor(SID(name)); }
        TexturePtr          getTexture(CString name)   const { return getTexture(SID(name)); }
        DataType            getDataType(CString name)  const { return getDataType(SID(name)); }

        //----------------------------------------------------------------------
        void setInt(StringID name, I32 val);
        void setFloat(StringID name, F32 val);
        void setVec4(StringID name, const Math::Vec4& vec);
        void setMatrix(StringID name, const DirectX::XMMATRIX& matrix);
        void setColor(StringID name, Color color);
        void setTexture(StringID name, TexturePtr tex);

        inline void setInt(CString name, I32 val)                           { setInt(SID(name), val); }
        inline void setFloat(CString name, F32 val)                         { setFloat(SID(name), val); }
        inline void setVec4(CString name, const Math::Vec4& vec)            { setVec4(SID(name), vec); }
        inline void setMatrix(CString name, const DirectX::XMMATRIX& matrix){ setMatrix(SID(name), matrix); }
        inline void setColor(CString name, Color color)                     { setColor(SID(name), color); }
        inline void setTexture(CString name, TexturePtr tex)                { setTexture(SID(name), tex); }

    protected:
        ShaderPtr   m_shader = nullptr;
        String      m_name = "NO NAME";

        // Data maps
        HashMap<StringID, I32>                          m_intMap;
        HashMap<StringID, F32>                          m_floatMap;
        HashMap<StringID, Math::Vec4>                   m_vec4Map;
        HashMap<StringID, DirectX::XMMATRIX>            m_matrixMap;
        HashMap<StringID, TexturePtr>                   m_textureMap;

        // Stores which property exist along his type
        HashMap<StringID, DataType>                     m_typeMap;

        // Each API should decide themselves how to efficiently update their data
        // @Return:
        //  True, if the uniform/resource exists in the materials shader.
        virtual bool _SetInt(StringID name, I32 val) = 0;
        virtual bool _SetFloat(StringID name, F32 val) = 0;
        virtual bool _SetVec4(StringID name, const Math::Vec4& vec) = 0;
        virtual bool _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) = 0;
        virtual bool _SetTexture(StringID name, TexturePtr texture) = 0;

    private:
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        virtual void bind() = 0;
        virtual void _ChangedShader() = 0;

        //----------------------------------------------------------------------
        IMaterial(const IMaterial& other)               = delete;
        IMaterial& operator = (const IMaterial& other)  = delete;
        IMaterial(IMaterial&& other)                    = delete;
        IMaterial& operator = (IMaterial&& other)       = delete;
    };

    using Material = IMaterial;

} // End namespaces

using MaterialPtr = std::shared_ptr<Graphics::IMaterial>;