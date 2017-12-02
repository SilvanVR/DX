#pragma once
/**********************************************************************
    class: Swapchain (D3D11Swapchain.h)

    author: S. Hau
    date: December 2, 2017
**********************************************************************/

#include "D3D11.hpp"

namespace Core { namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Swapchain
    {
    public:
        Swapchain(HWND hwnd, U16 width, U16 height, U8 numMSAASamples = 4);
        ~Swapchain();

        //----------------------------------------------------------------------
        ID3D11RenderTargetView* getRenderTargetView() { return m_pRenderTargetView; }
        ID3D11DepthStencilView* getDepthStencilView() { return m_pDepthStencilView; }

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
        // Clears the backbuffer with the given color, depth and stencil data.
        //----------------------------------------------------------------------
        void clear(Color color, F32 depth, U8 stencil);

        //----------------------------------------------------------------------
        // @Return: True if the given MSAA samples are supported for this swapchain.
        //----------------------------------------------------------------------
        bool numMSAASamplesSupported(U8 numSamples);

    private:
        U8                      m_msaaCount             = 1;
        U8                      m_msaaQualityLevel      = 0;

        //----------------------------------------------------------------------
        IDXGISwapChain1*        m_pSwapChain            = nullptr;
        ID3D11RenderTargetView* m_pRenderTargetView     = nullptr;
        ID3D11Texture2D*        m_pDepthStencilBuffer   = nullptr;
        ID3D11DepthStencilView* m_pDepthStencilView     = nullptr;

        //----------------------------------------------------------------------
        void _CreateD3D11Swapchain(HWND hwnd, U16 width, U16 height);
        void _CreateRenderTargetView();
        void _CreateDepthBuffer(U16 width, U16 height);

        //----------------------------------------------------------------------
        Swapchain(const Swapchain& other)               = delete;
        Swapchain& operator = (const Swapchain& other)  = delete;
        Swapchain(Swapchain&& other)                    = delete;
        Swapchain& operator = (Swapchain&& other)       = delete;
    };


} } } // End namespaces