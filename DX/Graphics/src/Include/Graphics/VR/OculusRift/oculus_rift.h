#pragma once
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "../vr.h"
#include "LibOVR/OVR_CAPI.h"

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    // @Return: True when LibOVR has been initialized.
    //----------------------------------------------------------------------
    bool OVRIsInitialized();

    //----------------------------------------------------------------------
    // @Return: Connected OvrControllerType OR'ed together
    //----------------------------------------------------------------------
    U32 GetConnectedController();

    //----------------------------------------------------------------------
    // @Return: Struct describing current input state from controller input.
    //----------------------------------------------------------------------
    ovrInputState GetOVRInputState(ovrControllerType controllerType);

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
    // Precondition: LibOVR is ALREADY initialized
    //**********************************************************************
    class OculusRift : public HMD
    {
    public:
        OculusRift();
        virtual ~OculusRift();

        //----------------------------------------------------------------------
        ovrGraphicsLuid getGraphicsLuid()   const { return m_graphicsLuid; }
        ovrSession      getSession()        const;

        //----------------------------------------------------------------------
        // VRDevice Interface
        //----------------------------------------------------------------------
        bool                    good() const override;
        void                    clear(Color col) override;
        void                    distortAndPresent(I64 frameIndex) override;
        DirectX::XMMATRIX       getProjection(Eye eye, F32 zNear, F32 zFar) const override;
        void                    bindForRendering(Eye eye) override;
        void                    setPerformanceHUD(PerfHudMode mode) override;
        bool                    isMounted() override;

    protected:
        OculusSwapchain*    m_eyeBuffers[2];

    private:
        ovrRecti            m_eyeRenderViewport[2];
        ovrEyeRenderDesc    m_eyeRenderDesc[2];
        ovrHmdDesc          m_HMDInfo;
        ovrPosef            m_currentEyeRenderPose[2];
        ovrGraphicsLuid     m_graphicsLuid;

        //----------------------------------------------------------------------
        bool _CreateSession();
        void _SetupDescription(const ovrHmdDesc& hmdInfo);
        void _UpdateEyeAndTouchPoses(I64 frameIndex);

        NULL_COPY_AND_ASSIGN(OculusRift)
    };

} } // End namespaces