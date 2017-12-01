#pragma once
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.h)

    author: S. Hau
    date: November 28, 2017

    D3D11 Renderer.
**********************************************************************/

#include "../i_renderer.h"

namespace Core { namespace Graphics {


    //**********************************************************************
    // D3D11 Renderer Subsystem.
    //**********************************************************************
    class D3D11Renderer : public IRenderer
    {
    public:
        D3D11Renderer(OS::Window* window);

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // IRenderer Interface
        //----------------------------------------------------------------------
        void render() override;
        void setVSync(bool enabled) override;
        void setMultiSampleCount(U32 numSamples) override;

    private:
        U32 m_multiSampleCount = 1;

        //----------------------------------------------------------------------
        void _InitD3D11();
        void _DeinitD3D11();

        void _CreateDeviceAndContext();
        void _CreateSwapchain();

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