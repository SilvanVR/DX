#pragma once
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "../vr.h"

// Include API dependant headers
#define   OVR_D3D_VERSION 11
#include "LibOVR/OVR_CAPI_D3D.h"
#include "LibOVR/OVR_CAPI_Vk.h"

#include "../../D3D11/D3D11.hpp"
#include "../../enums.hpp"
#include <array>

namespace Graphics { namespace VR {

    //**********************************************************************
    class OculusSwapchain 
    {
    public:
        OculusSwapchain() = default;
        virtual ~OculusSwapchain() {}

        //----------------------------------------------------------------------
        const ovrTextureSwapChain& get() const { return m_swapChain; }

        //----------------------------------------------------------------------
        // Commits the texture in the swapchain.
        //----------------------------------------------------------------------
        void commit(ovrSession session) { ovr_CommitTextureSwapChain(session, m_swapChain); }

        //----------------------------------------------------------------------
        void release(ovrSession session) { ovr_DestroyTextureSwapChain(session, m_swapChain); }

        //----------------------------------------------------------------------
        virtual void bindForRendering(ovrSession session) = 0;

        //----------------------------------------------------------------------
        virtual void clear(ovrSession session, Color color) = 0;

    protected:
        ovrTextureSwapChain m_swapChain;

        NULL_COPY_AND_ASSIGN(OculusSwapchain)
    };

    //**********************************************************************
    class OculusSwapchainDX : public OculusSwapchain
    {
    public:
        OculusSwapchainDX(ovrSession session, I32 sizeW, I32 sizeH);

        //----------------------------------------------------------------------
        // OculusSwapchain Interface
        //----------------------------------------------------------------------
        void bindForRendering(ovrSession session) override;
        void clear(ovrSession session, Color color) override;

    private:
        ComPtr<ID3D11RenderTargetView> m_texRtv[3];
        ID3D11RenderTargetView* _GetRTVDX(ovrSession session)
        {
            int index = 0;
            ovr_GetTextureSwapChainCurrentIndex( session, m_swapChain, &index );
            return m_texRtv[index];
        }
    };

    //**********************************************************************
    // Precondition: LibOVR is ALREADY initialized
    //**********************************************************************
    class OculusRift : public HMD
    {
    public:
        OculusRift(API api);
        ~OculusRift();

        //----------------------------------------------------------------------
        // VRDevice Interface
        //----------------------------------------------------------------------
        bool                    hasFocus() override;
        void                    clear(Color col) override { for (auto eye : {LeftEye, RightEye}) m_eyeBuffers[eye]->clear(m_session, col); }
        void                    distortAndPresent(I64 frameIndex) override;
        std::array<EyePose, 2>  getEyePoses() const override;
        std::array<EyePose, 2>  calculateEyePoses(I64 frameIndex) override;
        void                    bindForRendering(Eye eye) override;
        void                    setPerformanceHUD(PerfHudMode mode) override;
        bool                    isMounted() override;

    private:
        ovrSession          m_session;
        ovrRecti            m_eyeRenderViewport[2];
        ovrEyeRenderDesc    m_eyeRenderDesc[2];
        ovrHmdDesc          m_HMDInfo;
        ovrPosef            m_currentEyeRenderPose[2];
        OculusSwapchain*    m_eyeBuffers[2];
        bool                m_calculatedEyePoses = false;

        //----------------------------------------------------------------------
        bool _CreateSession();
        void _CreateEyeBuffers(API api, const ovrHmdDesc& hmdInfo);
        void _SetupDescription(const ovrHmdDesc& hmdInfo);

        NULL_COPY_AND_ASSIGN(OculusRift)
    };

} } // End namespaces