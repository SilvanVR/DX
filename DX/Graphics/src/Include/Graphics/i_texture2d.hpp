#pragma once
/**********************************************************************
    class: Texture2D (i_texture2d.hpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "i_texture.h"

namespace Graphics
{

    //**********************************************************************
    class ITexture2D : public ITexture
    {
    public:
        ITexture2D(U32 width, U32 height, TextureFormat format, bool generateMips)
            : ITexture( width, height, format ), m_generateMips(generateMips)
        {
            if (m_generateMips)
                this->generateMips();
            m_pixels = new Color[m_width * m_height];
        }
        virtual ~ITexture2D() { SAFE_DELETE( m_pixels ); }

        //----------------------------------------------------------------------
        void setPixel(U32 x, U32 y, Color color) { ((Color*)m_pixels)[x + y * m_width] = color; }
        void setPixels(const void* pPixels) { memcpy(m_pixels, pPixels, m_width * m_height * sizeof(Color)); }
        void generateMips() { m_generateMips = true; m_mipCount = (U32)floor(log2(std::min(m_width, m_height))) + 1; }

    protected:
        bool                m_generateMips = true;

        // Heap allocated mem for pixels. How large it is depends on width/height and the format
        void*               m_pixels = nullptr;

    private:
        //----------------------------------------------------------------------
        ITexture2D(const ITexture2D& other)               = delete;
        ITexture2D& operator = (const ITexture2D& other)  = delete;
        ITexture2D(ITexture2D&& other)                    = delete;
        ITexture2D& operator = (ITexture2D&& other)       = delete;
    };

    using Texture2D = ITexture2D;

}