#pragma once
/**********************************************************************
    class: IBindableTexture (D3D11IBindableTexture.h)

    author: S. Hau
    date: March 30, 2018

    Interface for all textures in Direct3D11 which can be bound to a shader.
**********************************************************************/

#include "enums.hpp"
#include "../D3D11Utility.h"
#include "shader_resources.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class IBindableTexture
    {
    public:
        IBindableTexture() = default;
        virtual ~IBindableTexture();

        //----------------------------------------------------------------------
        // Default implementation is provided, but can be overriden if desired.
        //----------------------------------------------------------------------
        virtual void bind(const ShaderResourceDeclaration& res);

        //----------------------------------------------------------------------
        // @Return:
        //  D3D11Texture for this texture.
        //----------------------------------------------------------------------
        ID3D11Texture2D* getD3D11Texture() { return m_pTexture; }

    protected:
        ID3D11SamplerState*         m_pSampleState      = nullptr;
        ID3D11Texture2D*            m_pTexture          = nullptr;
        ID3D11ShaderResourceView*   m_pTextureView      = nullptr;

        bool                        m_gpuUpToDate      = true;
        bool                        m_generateMips     = true;
        bool                        m_keepPixelsInRAM  = false;

        //----------------------------------------------------------------------
        void _CreateSampler(U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode);

        //----------------------------------------------------------------------
        // Pushes the pixel data to the GPU before binding if gpu is not up to date.
        //----------------------------------------------------------------------
        virtual void _PushToGPU() {}

    private:
        //----------------------------------------------------------------------
        IBindableTexture(const IBindableTexture& other)               = delete;
        IBindableTexture& operator = (const IBindableTexture& other)  = delete;
        IBindableTexture(IBindableTexture&& other)                    = delete;
        IBindableTexture& operator = (IBindableTexture&& other)       = delete;
    };

} } // End namespaces