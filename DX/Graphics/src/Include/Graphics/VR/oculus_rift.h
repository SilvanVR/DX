#pragma once
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "vr.h"

// Include API dependant headers
#define   OVR_D3D_VERSION 11
#include "LibOVR/OVR_CAPI_D3D.h"
#include "LibOVR/OVR_CAPI_Vk.h"

#include "../D3D11/D3D11.hpp"
#include "../structs.hpp"
#include "../enums.hpp"
#include <array>

namespace Graphics { namespace VR {

    //**********************************************************************
    class OculusSwapchain 
    {
    public:
        OculusSwapchain(API api, ovrSession session, I32 sizeW, I32 sizeH);

        const ovrTextureSwapChain& get() const { return m_swapChain; }

        //----------------------------------------------------------------------
        void commit(ovrSession session)
        {
            ovr_CommitTextureSwapChain(session, m_swapChain);
        }

        //----------------------------------------------------------------------
        void release(ovrSession session)
        {
            ovr_DestroyTextureSwapChain(session, m_swapChain);
        }

        //----------------------------------------------------------------------
        void clear(ovrSession session, Color color);

    private:
        ovrTextureSwapChain m_swapChain;

        // D3D11
        ComPtr<ID3D11RenderTargetView> m_texRtv[3];
        ID3D11RenderTargetView* _GetRTVDX(ovrSession session)
        {
            int index = 0;
            ovr_GetTextureSwapChainCurrentIndex(session, m_swapChain, &index);
            return m_texRtv[index];
        }
    };

    //**********************************************************************
    class OculusRift
    {
    public:
        OculusRift(API api);
        ~OculusRift();

        std::array<DirectX::XMMATRIX, 2> getEyeMatrices(I64 frameIndex);

        //----------------------------------------------------------------------
        // @Return: Whether this HMD was sucessfully initialized.
        //----------------------------------------------------------------------
        bool isInitialized() const { return m_initialized; }

        //----------------------------------------------------------------------
        // @Return: TODO
        //----------------------------------------------------------------------
        bool isVisible() const { return m_isVisible; }

        //----------------------------------------------------------------------
        // @Return: Viewport matching the resolution from the HMD for the given eye.
        //----------------------------------------------------------------------
        ViewportRect getViewport(Eye eye) { return { (F32)m_eyeRenderViewport[eye].Pos.x, (F32)m_eyeRenderViewport[eye].Pos.y, (F32)m_eyeRenderViewport[eye].Size.w, (F32)m_eyeRenderViewport[eye].Size.h }; }

        //----------------------------------------------------------------------
        // Clears the next swapchain texture for the given eye with the given color.
        //----------------------------------------------------------------------
        void clear(Eye eye, Color col) { m_eyeBuffers[eye]->clear( m_session, col ); }

        //----------------------------------------------------------------------
        // Distorts the swapchain images and presents them to the HMD.
        //----------------------------------------------------------------------
        void distortAndPresent(I64 frameIndex);

    private:
        ovrSession          m_session;
        ovrRecti            m_eyeRenderViewport[2];
        ovrEyeRenderDesc    m_eyeRenderDesc[2];
        ovrHmdDesc          m_HMDInfo;
        ovrPosef            m_currentEyeRenderPose[2];
        OculusSwapchain*    m_eyeBuffers[2];
        bool                m_isVisible = true;
        bool                m_initialized = false;

        //----------------------------------------------------------------------
        bool _InitLibOVR();
        void _CreateEyeBuffers(API api, const ovrHmdDesc& hmdInfo);

        NULL_COPY_AND_ASSIGN(OculusRift)
    };

} } // End namespaces