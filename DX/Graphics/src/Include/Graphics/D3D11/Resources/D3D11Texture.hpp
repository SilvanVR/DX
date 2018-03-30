#pragma once
/**********************************************************************
    class: D3D11Texture (D3D11Texture.hpp)

    author: S. Hau
    date: March 30, 2018

    Defines interface for all texture types in d3d11.
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class D3D11Texture
    {
    public:
        D3D11Texture() = default;
        virtual ~D3D11Texture() { }

        //----------------------------------------------------------------------
        virtual void bind(U32 slot) = 0;

    private:
        //----------------------------------------------------------------------
        D3D11Texture(const D3D11Texture& other)               = delete;
        D3D11Texture& operator = (const D3D11Texture& other)  = delete;
        D3D11Texture(D3D11Texture&& other)                    = delete;
        D3D11Texture& operator = (D3D11Texture&& other)       = delete;
    };

} } // End namespaces