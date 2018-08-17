#pragma once
/**********************************************************************
    class: Swapchain (D3D11Swapchain.h)

    author: S. Hau
    date: December 2, 2017
**********************************************************************/

#include "D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Swapchain
    {
    public:
        Swapchain(HWND hwnd, U16 width, U16 height, U8 numMSAASamples = 4);

        //----------------------------------------------------------------------
        // Bind buffer to the output merger.
        //----------------------------------------------------------------------
        void bindForRendering();

        //----------------------------------------------------------------------
        // Simply recreates the swapchain buffers with the new width and height.
        //----------------------------------------------------------------------
        void recreate(U16 width, U16 height);

        //----------------------------------------------------------------------
        // Present the backbuffer to the screen.
        // @Params:
        //  "vsync": If true, it waits for the next vertical blink before presenting.
        //----------------------------------------------------------------------
        void present(bool vsync);

        //----------------------------------------------------------------------
        // Clears the backbuffer with the given color.
        //----------------------------------------------------------------------
        void clear(Color color);

    private:
        ComPtr<IDXGISwapChain1>        m_pSwapChain            = nullptr;
        ComPtr<ID3D11RenderTargetView> m_pRenderTargetView     = nullptr;

        //----------------------------------------------------------------------
        void _CreateD3D11Swapchain(HWND hwnd, U16 width, U16 height, U8 msaaCount, U8 msaaQualityLevel);
        void _CreateRenderTargetView();

        NULL_COPY_AND_ASSIGN(Swapchain)
    };


} } // End namespaces