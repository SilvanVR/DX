#pragma once
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.h)

    author: S. Hau
    date: November 28, 2017

    D3D11 Renderer.
**********************************************************************/

#include "../i_renderer.h"
#include "D3D11.hpp"
#include "D3D11Swapchain.h"

namespace Core { namespace Graphics {

    //**********************************************************************
    // D3D11 Renderer Subsystem.
    //**********************************************************************
    class D3D11Renderer : public IRenderer
    {
        static const U8 INITIAL_MSAA_SAMPLES = 4;

    public:
        D3D11Renderer(OS::Window* window) : IRenderer( window ) {}

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void render() override;
        void setVSync(bool enabled) override { m_vsync = enabled; }
        void setClearColor(Color clearColor) { m_clearColor = clearColor; }
        void setMultiSampleCount(U32 numSamples) override;

    private:
        D3D11::Swapchain*   m_pSwapchain    = nullptr;
        Color               m_clearColor    = Color::BLACK;
        bool                m_vsync         = false;

        //----------------------------------------------------------------------
        void _InitD3D11();
        void _DeinitD3D11();

        void _CreateDeviceAndContext();
        void _CreateSwapchain( U32 numSamples );

        void _ReportLiveObjects();

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void OnWindowSizeChanged(U16 w, U16 h) override;

        //----------------------------------------------------------------------
        D3D11Renderer(const D3D11Renderer& other)               = delete;
        D3D11Renderer& operator = (const D3D11Renderer& other)  = delete;
        D3D11Renderer(D3D11Renderer&& other)                    = delete;
        D3D11Renderer& operator = (D3D11Renderer&& other)       = delete;
    };




} } // End namespaces