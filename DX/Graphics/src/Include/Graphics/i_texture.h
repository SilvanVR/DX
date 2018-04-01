#pragma once
/**********************************************************************
    class: Texture (texture.h)

    author: S. Hau
    date: March 24, 2018

**********************************************************************/

#include "enums.hpp"

namespace Graphics
{

    //**********************************************************************
    class ITexture
    {
    public:
        ITexture() = default;
        virtual ~ITexture() = default;

        //----------------------------------------------------------------------
        F32                  getAspectRatio()    const { return (F32)getWidth() / getHeight(); }
        U32                  getWidth()          const { return m_width; }
        U32                  getHeight()         const { return m_height; }
        U32                  getMipCount()       const { return m_mipCount; }
        U32                  getAnisoLevel()     const { return m_anisoLevel; }
        TextureFormat        getFormat()         const { return m_format; }
        TextureDimension     getDimension()      const { return m_dimension; }
        TextureFilter        getFilter()         const { return m_filter; }
        TextureAddressMode   getClampMode()      const { return m_clampMode; }

        //----------------------------------------------------------------------
        // Set the filter mode for this texture. Note that this has no effect if
        // aniso-level is greater than 1.
        //----------------------------------------------------------------------
        void setFilter(TextureFilter filter)            { m_filter = filter; _UpdateSampler(); }
        void setClampMode(TextureAddressMode clampMode) { m_clampMode = clampMode; _UpdateSampler(); }
        void setAnisoLevel(U32 level)                   { m_anisoLevel = level; _UpdateSampler(); }

    protected:
        U32                 m_width             = 0;
        U32                 m_height            = 0;
        U32                 m_mipCount          = 1;
        U32                 m_anisoLevel        = 1;
        TextureFormat       m_format            = TextureFormat::Unknown;
        TextureDimension    m_dimension         = TextureDimension::Unknown;
        TextureFilter       m_filter            = TextureFilter::Trilinear;
        TextureAddressMode  m_clampMode         = TextureAddressMode::Repeat;

        //----------------------------------------------------------------------
        void _Init(U32 width, U32 height, TextureFormat format) { m_width = width; m_height = height; m_format = format; }

        // Indicates that something related to sampling has been changed. 
        virtual void _UpdateSampler() = 0;

        //----------------------------------------------------------------------
        void _UpdateMipCount() { m_mipCount = static_cast<U32>( std::floor( std::log2( std::min( m_width, m_height) ) ) + 1 ); }

    private:
        //----------------------------------------------------------------------
        ITexture(const ITexture& other)               = delete;
        ITexture& operator = (const ITexture& other)  = delete;
        ITexture(ITexture&& other)                    = delete;
        ITexture& operator = (ITexture&& other)       = delete;
    };

    using Texture = ITexture;
}