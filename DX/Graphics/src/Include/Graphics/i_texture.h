#pragma once
/**********************************************************************
    class: Texture (texture.h)

    author: S. Hau
    date: March 24, 2018

**********************************************************************/

#include "enums.hpp"
#include "shader_resources.hpp"
#include <cmath>

namespace Graphics
{

    //**********************************************************************
    class ITexture
    {
    public:
        ITexture(TextureDimension dimension) : m_dimension(dimension) {}
        virtual ~ITexture() = default;

        //----------------------------------------------------------------------
        U32                  getWidth()          const { return m_width; }
        U32                  getHeight()         const { return m_height; }
        F32                  getAspectRatio()    const { return (F32)getWidth() / getHeight(); }
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

        //----------------------------------------------------------------------
        // Pointer to the underlying graphics resource.
        //----------------------------------------------------------------------
        virtual U64* getNativeTexturePtr() const = 0;

    protected:
        U32                 m_width             = 0;
        U32                 m_height            = 0;
        U32                 m_mipCount          = 1;
        U32                 m_anisoLevel        = 8;
        TextureFormat       m_format            = TextureFormat::Unknown;
        TextureDimension    m_dimension         = TextureDimension::Unknown;
        TextureFilter       m_filter            = TextureFilter::Trilinear;
        TextureAddressMode  m_clampMode         = TextureAddressMode::Repeat;

        //----------------------------------------------------------------------
        void _Init(TextureDimension dimension, U32 width, U32 height, TextureFormat format) { m_dimension = dimension; m_width = width; m_height = height; m_format = format; }
        void _UpdateMipCount() { m_mipCount = static_cast<U32>( std::floor( std::log2( std::min( m_width, m_height) ) ) + 1 ); }

        //----------------------------------------------------------------------
        // Indicates that something related to sampling has been changed.
        //----------------------------------------------------------------------
        virtual void _UpdateSampler() = 0;

    private:
        friend class IMaterial;
        friend class IShader;
        friend class IRenderTexture;
        friend class D3D11Renderer;
        friend class VkRenderer;
        virtual void bind(const ShaderResourceDeclaration& res) = 0;

        NULL_COPY_AND_ASSIGN(ITexture)
    };

    using Texture = ITexture;

} // End namespaces

using TexturePtr = std::shared_ptr<Graphics::ITexture>;