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
        ~Swapchain();

        //----------------------------------------------------------------------
        // Bind the color + depth attachment to the output merger.
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
        U8                      m_msaaCount             = 1;
        U8                      m_msaaQualityLevel      = 0;

        //----------------------------------------------------------------------
        IDXGISwapChain1*        m_pSwapChain            = nullptr;
        ID3D11RenderTargetView* m_pRenderTargetView     = nullptr;

        //----------------------------------------------------------------------
        void _CreateD3D11Swapchain(HWND hwnd, U16 width, U16 height);
        void _CreateRenderTargetView();

        //----------------------------------------------------------------------
        Swapchain(const Swapchain& other)               = delete;
        Swapchain& operator = (const Swapchain& other)  = delete;
        Swapchain(Swapchain&& other)                    = delete;
        Swapchain& operator = (Swapchain&& other)       = delete;
    };


} } // End namespaces