#pragma once
/**********************************************************************
    class: Texture (texture.h)

    author: S. Hau
    date: March 24, 2018

**********************************************************************/

namespace Graphics
{
    //----------------------------------------------------------------------
    enum class Dimension
    {
        UNKNOWN = 0,
        Tex2D,
        Tex3D,
        Cube,
        Tex2DArray,
        CubeArray
    };

    //----------------------------------------------------------------------
    enum class TextureFormat
    {
        UNKNOWN = 0,
        R32G32B32A32_TYPELESS = 1,
        R32G32B32A32_FLOAT = 2,
        R32G32B32A32_UINT = 3,
        R32G32B32A32_SINT = 4,
        R32G32B32_TYPELESS = 5,
        R32G32B32_FLOAT = 6,
        R32G32B32_UINT = 7,
        R32G32B32_SINT = 8,
        R16G16B16A16_TYPELESS = 9,
        R16G16B16A16_FLOAT = 10,
        R16G16B16A16_UNORM = 11,
        R16G16B16A16_UINT = 12,
        R16G16B16A16_SNORM = 13,
        R16G16B16A16_SINT = 14,
        R32G32_TYPELESS = 15,
        R32G32_FLOAT = 16,
        R32G32_UINT = 17,
        R32G32_SINT = 18,
        R32G8X24_TYPELESS = 19,
        D32_FLOAT_S8X24_UINT = 20,
        R32_FLOAT_X8X24_TYPELESS = 21,
        X32_TYPELESS_G8X24_UINT = 22,
        R10G10B10A2_TYPELESS = 23,
        R10G10B10A2_UNORM = 24,
        R10G10B10A2_UINT = 25,
        R11G11B10_FLOAT = 26,
        R8G8B8A8_TYPELESS = 27,
        R8G8B8A8_UNORM = 28,
        R8G8B8A8_UNORM_SRGB = 29,
        R8G8B8A8_UINT = 30,
        R8G8B8A8_SNORM = 31,
        R8G8B8A8_SINT = 32,
        R16G16_TYPELESS = 33,
        R16G16_FLOAT = 34,
        R16G16_UNORM = 35,
        R16G16_UINT = 36,
        R16G16_SNORM = 37,
        R16G16_SINT = 38,
        R32_TYPELESS = 39,
        D32_FLOAT = 40,
        R32_FLOAT = 41,
        R32_UINT = 42,
        R32_SINT = 43,
        R24G8_TYPELESS = 44,
        D24_UNORM_S8_UINT = 45,
        R24_UNORM_X8_TYPELESS = 46,
        X24_TYPELESS_G8_UINT = 47,
        R8G8_TYPELESS = 48,
        R8G8_UNORM = 49,
        R8G8_UINT = 50,
        R8G8_SNORM = 51,
        R8G8_SINT = 52,
        R16_TYPELESS = 53,
        R16_FLOAT = 54,
        D16_UNORM = 55,
        R16_UNORM = 56,
        R16_UINT = 57,
        R16_SNORM = 58,
        R16_SINT = 59,
        R8_TYPELESS = 60,
        R8_UNORM = 61,
        R8_UINT = 62,
        R8_SNORM = 63,
        R8_SINT = 64,
        A8_UNORM = 65,
        R1_UNORM = 66,
        R9G9B9E5_SHAREDEXP = 67,
        R8G8_B8G8_UNORM = 68,
        G8R8_G8B8_UNORM = 69,
        BC1_TYPELESS = 70,
        BC1_UNORM = 71,
        BC1_UNORM_SRGB = 72,
        BC2_TYPELESS = 73,
        BC2_UNORM = 74,
        BC2_UNORM_SRGB = 75,
        BC3_TYPELESS = 76,
        BC3_UNORM = 77,
        BC3_UNORM_SRGB = 78,
        BC4_TYPELESS = 79,
        BC4_UNORM = 80,
        BC4_SNORM = 81,
        BC5_TYPELESS = 82,
        BC5_UNORM = 83,
        BC5_SNORM = 84,
        B5G6R5_UNORM = 85,
        B5G5R5A1_UNORM = 86,
        B8G8R8A8_UNORM = 87,
        B8G8R8X8_UNORM = 88,
        R10G10B10_XR_BIAS_A2_UNORM = 89,
        B8G8R8A8_TYPELESS = 90,
        B8G8R8A8_UNORM_SRGB = 91,
        B8G8R8X8_TYPELESS = 92,
        B8G8R8X8_UNORM_SRGB = 93,
        BC6H_TYPELESS = 94,
        BC6H_UF16 = 95,
        DXGI_FORMAT_BC6H_SF16 = 96,
        DXGI_FORMAT_BC7_TYPELESS = 97,
        DXGI_FORMAT_BC7_UNORM = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB = 99,
    };

    //**********************************************************************
    class ITexture
    {
    public:
        ITexture() = default;
        virtual ~ITexture(){ SAFE_DELETE( m_pixels ); }

        //----------------------------------------------------------------------
        void generateMips() {}

        //----------------------------------------------------------------------
        inline F32              getAspectRatio()    const { return (F32)getWidth() / getHeight(); }
        inline U32              getWidth()          const { return m_width; }
        inline U32              getHeight()         const { return m_height; }
        inline TextureFormat    getFormat()         const { return m_format; }
        inline U32              getMipCount()       const { return m_mipCount; }
        inline Dimension        getDimension()      const { return m_dimension; }

        //----------------------------------------------------------------------
        void setSize(U32 width, U32 height) { m_width = width; m_height = height; m_pixels = new Color[m_width * m_height]; }

        void setPixel(U32 x, U32 y, Color color) { ((Color*)m_pixels)[x + y * m_width] = color; }
        void setPixels(const void* pPixels) { memcpy( m_pixels, pPixels, m_width * m_height * sizeof(Color) ); }

        virtual void init() = 0;
        virtual void apply() = 0;

        //public bool Resize(int width, int height, TextureFormat format, bool hasMipMap);

    protected:
        U32             m_width     = 0;
        U32             m_height    = 0;
        U32             m_mipCount  = 1;
        TextureFormat   m_format    = TextureFormat::UNKNOWN;
        Dimension       m_dimension = Dimension::UNKNOWN;

        // Heap allocated mem for pixels. How large it is depends on width/height and the format
        void*           m_pixels    = nullptr;

    private:
        //----------------------------------------------------------------------
        ITexture(const ITexture& other)               = delete;
        ITexture& operator = (const ITexture& other)  = delete;
        ITexture(ITexture&& other)                    = delete;
        ITexture& operator = (ITexture&& other)       = delete;
    };

    using Texture = ITexture;
}