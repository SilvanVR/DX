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
        virtual ~ITexture() { SAFE_DELETE( m_pixels ); }

        //----------------------------------------------------------------------
        void generateMips() {}

        //----------------------------------------------------------------------
        inline F32              getAspectRatio()    const { return (F32)getWidth() / getHeight(); }
        inline U32              getWidth()          const { return m_width; }
        inline U32              getHeight()         const { return m_height; }
        inline U32              getMipCount()       const { return m_mipCount; }
        inline U32              getAnisoLevel()     const { return m_anisoLevel; }
        inline TextureFormat    getFormat()         const { return m_format; }
        inline TextureDimension getDimension()      const { return m_dimension; }
        inline TextureFilter    getFilter()         const { return m_filter; }
        inline TextureClampMode getClampMode()      const { return m_clampMode; }

        //----------------------------------------------------------------------
        void setFilter(TextureFilter filter)            { m_filter = filter; _UpdateSampler(); }
        void setClampMode(TextureClampMode clampMode)   { m_clampMode = clampMode; _UpdateSampler(); }
        void setAnisoLevel(U32 level)                   { m_anisoLevel = level; _UpdateSampler(); }

        //----------------------------------------------------------------------
        void setSize(U32 width, U32 height) { m_width = width; m_height = height; m_pixels = new Color[m_width * m_height]; }

        void setPixel(U32 x, U32 y, Color color) { ((Color*)m_pixels)[x + y * m_width] = color; }
        void setPixels(const void* pPixels) { memcpy( m_pixels, pPixels, m_width * m_height * sizeof(Color) ); }

        virtual void init() = 0;
        virtual void apply() = 0;

        //public bool Resize(int width, int height, TextureFormat format, bool hasMipMap);

    protected:
        U32                 m_width             = 0;
        U32                 m_height            = 0;
        U32                 m_mipCount          = 1;
        U32                 m_anisoLevel        = 1;
        TextureFormat       m_format            = TextureFormat::UNKNOWN;
        TextureDimension    m_dimension         = TextureDimension::UNKNOWN;
        TextureFilter       m_filter            = TextureFilter::Trilinear;
        TextureClampMode    m_clampMode         = TextureClampMode::Repeat;

        // Heap allocated mem for pixels. How large it is depends on width/height and the format
        void*               m_pixels    = nullptr;

        // Indicates that something related to sampling has been changed. 
        virtual void _UpdateSampler() = 0;

    private:
        //----------------------------------------------------------------------
        ITexture(const ITexture& other)               = delete;
        ITexture& operator = (const ITexture& other)  = delete;
        ITexture(ITexture&& other)                    = delete;
        ITexture& operator = (ITexture&& other)       = delete;
    };

    using Texture = ITexture;
}