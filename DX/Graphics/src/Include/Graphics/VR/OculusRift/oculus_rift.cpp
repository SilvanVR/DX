#include "oculus_rift.h"
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "Logging/logging.h"
#include "Events/event_dispatcher.h"

//----------------------------------------------------------------------
inline Math::Vec3 ConvertVec3( const ovrVector3f& vec3 )
{
    // Transform from RH to LH coordinate system.
    return Math::Vec3( vec3.x, vec3.y, -vec3.z );
}

//----------------------------------------------------------------------
inline Math::Quat ConvertQuat( const ovrQuatf& q )
{
    // Transform from RH to LH. Rotation about X + Y axis must be flipped. 
    // Awkardly z-rotation in a LH coordinate system is already flipped (CCW) whereas X+Y are CW (This took me hours to figure out...)
    return Math::Quat( -q.x, -q.y, q.z, q.w );
}


namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    ovrSession g_session = nullptr;

    //----------------------------------------------------------------------
    bool OVRIsInitialized()
    {
        return g_session != nullptr;
    }

    //----------------------------------------------------------------------
    U32 GetConnectedController()
    {
        return ovr_GetConnectedControllerTypes( g_session );
    }

    //----------------------------------------------------------------------
    ovrInputState GetOVRInputState( ovrControllerType controllerType )
    {
        ovrInputState inputState;
        if ( not OVR_SUCCESS( ovr_GetInputState( g_session, controllerType, &inputState ) ) )
            LOG_WARN_RENDERING( "GetOVRInputState(): Failed to retrieve the input state. Does an ovr session exists?" );

        return inputState;
    }

    //----------------------------------------------------------------------
    OculusRift::OculusRift()
    {
        if ( not _CreateSession() )
        {
            LOG_WARN_RENDERING( "OculusRift: Failed to create a session. HMD is plugged in?" );
            return;
        }

        m_HMDInfo = ovr_GetHmdDesc( g_session );
        for (auto eye : { LeftEye, RightEye })
        {
            ovrSizei idealSize = ovr_GetFovTextureSize( g_session, (ovrEyeType)eye, m_HMDInfo.DefaultEyeFov[eye], 1.0f );
            m_eyeRenderViewport[eye].Pos.x = 0;
            m_eyeRenderViewport[eye].Pos.y = 0;
            m_eyeRenderViewport[eye].Size = idealSize;

            m_eyeRenderDesc[eye] = ovr_GetRenderDesc( g_session, (ovrEyeType)eye, m_HMDInfo.DefaultEyeFov[eye] );
        }

        _SetupDescription( m_HMDInfo );
    }

    //----------------------------------------------------------------------
    OculusRift::~OculusRift()
    {
        setPerformanceHUD( PerfHudMode::Off ); // Because the perf hud persists
        if (g_session)
        {
            for (auto eye : { LeftEye, RightEye })
            {
                m_eyeBuffers[eye]->release( g_session );
                delete m_eyeBuffers[eye];
            }
            ovr_Destroy( g_session );
            ovr_Shutdown();
        }
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    ovrSession OculusRift::getSession() const
    {
        return g_session;
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX OculusRift::getProjection( Eye eye, F32 zNear, F32 zFar ) const
    {
        ovrMatrix4f p = ovrMatrix4f_Projection( m_eyeRenderDesc[eye].Fov, zNear, zFar, ovrProjection_LeftHanded );
        auto proj = DirectX::XMMatrixSet( p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
                                          p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
                                          p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
                                          p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3] );
        return proj;
    }

    //----------------------------------------------------------------------
    void OculusRift::_UpdateEyeAndTouchPoses( I64 frameIndex )
    {
        // Focus check
        bool hasFocus = _HasFocus();
        if (m_hasFocus && not hasFocus)
        {
            m_hasFocus = false;
            Events::EventDispatcher::GetEvent( EVENT_HMD_FOCUS_LOST ).invoke();
        }
        else if (not m_hasFocus && hasFocus)
        {
            m_hasFocus = true;
            Events::EventDispatcher::GetEvent( EVENT_HMD_FOCUS_GAINED ).invoke();
        }

        ovr_WaitToBeginFrame( g_session, frameIndex );
        F64 ftiming = ovr_GetPredictedDisplayTime( g_session, frameIndex );
        ovrTrackingState hmdState = ovr_GetTrackingState( g_session, ftiming, ovrTrue );

        ovrPosef HmdToEyePose[2] = { m_eyeRenderDesc[0].HmdToEyePose, m_eyeRenderDesc[1].HmdToEyePose };
        ovr_CalcEyePoses( hmdState.HeadPose.ThePose, HmdToEyePose, m_currentEyeRenderPose );

        // HMD
        for (auto eye : { LeftEye, RightEye })
        {
            m_currentEyePoses[eye].position = ConvertVec3( m_currentEyeRenderPose[eye].Position ) / m_worldScale;
            m_currentEyePoses[eye].rotation = ConvertQuat( m_currentEyeRenderPose[eye].Orientation );
        }

        // Touch
        for (auto hand : { Hand::Left, Hand::Right })
        {
            m_currentTouchPoses[(I32)hand].position = ConvertVec3( hmdState.HandPoses[(I32)hand].ThePose.Position ) / m_worldScale;
            m_currentTouchPoses[(I32)hand].rotation = ConvertQuat( hmdState.HandPoses[(I32)hand].ThePose.Orientation );
        }
        ovr_BeginFrame( g_session, frameIndex );
    }

    //----------------------------------------------------------------------
    void OculusRift::bindForRendering( Eye eye )
    {
        m_eyeBuffers[eye]->bindForRendering( g_session );
    }

    //----------------------------------------------------------------------
    void OculusRift::distortAndPresent( I64 frameIndex )
    {
        if (m_currentFrameIndex != frameIndex)
            return;

        ovrLayerEyeFov ld;
        ld.Header.Type = ovrLayerType_EyeFov;
        ld.Header.Flags = 0;
        for (auto eye : { LeftEye, RightEye })
        {
            m_eyeBuffers[eye]->commit( g_session );
            ld.ColorTexture[eye]    = m_eyeBuffers[eye]->get();
            ld.Viewport[eye]        = m_eyeRenderViewport[eye];
            ld.Fov[eye]             = m_HMDInfo.DefaultEyeFov[eye];
            ld.RenderPose[eye]      = m_currentEyeRenderPose[eye];
        }

        ovrViewScaleDesc viewScaleDesc;
        viewScaleDesc.HmdSpaceToWorldScaleInMeters = 0.1f;
        viewScaleDesc.HmdToEyePose[0] = m_eyeRenderDesc[0].HmdToEyePose;
        viewScaleDesc.HmdToEyePose[1] = m_eyeRenderDesc[1].HmdToEyePose;

        ovrLayerHeader* layers = &ld.Header;
        ovrResult result = ovr_EndFrame( g_session, frameIndex, &viewScaleDesc, &layers, 1 );

        if (result == ovrError_DisplayLost)
            LOG_WARN_RENDERING( "OculusRift: HMD was disconnected from the computer." );
        else if (result != ovrSuccess)
            LOG_WARN_RENDERING( "OculusRift: Failed to submit frame to HMD." );
    }

    //----------------------------------------------------------------------
    bool OculusRift::good() const
    {
        return g_session != nullptr;
    }

    //----------------------------------------------------------------------
    void OculusRift::clear( Color col )
    { 
        for (auto eye : { LeftEye, RightEye }) 
            m_eyeBuffers[eye]->clear( g_session, col );
    }

    //----------------------------------------------------------------------
    void OculusRift::setPerformanceHUD( PerfHudMode mode )
    {
        switch (mode)
        {
            case PerfHudMode::Off: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_Off ); break;
            case PerfHudMode::PerfSummary: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_PerfSummary ); break;
            case PerfHudMode::LatencyTiming: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_LatencyTiming ); break;
            case PerfHudMode::AppRenderTiming: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_AppRenderTiming ); break;
            case PerfHudMode::CompRenderTiming: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_CompRenderTiming ); break;
            case PerfHudMode::AswStats: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_AswStats ); break;
            case PerfHudMode::VersionInfo: ovr_SetInt( g_session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_VersionInfo ); break;
        }
    }

    //----------------------------------------------------------------------
    bool OculusRift::isMounted()
    {
        ovrSessionStatus sessionStatus;
        ovr_GetSessionStatus( g_session, &sessionStatus );
        return sessionStatus.HmdMounted;
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    bool OculusRift::_CreateSession()
    {
        if (ovr_Create( &g_session, &m_graphicsLuid) != ovrSuccess)
            return false;

        return true;
    }

    //----------------------------------------------------------------------
    void OculusRift::_SetupDescription( const ovrHmdDesc& hmdInfo )
    {
        m_description.name        = hmdInfo.ProductName;
        m_description.resolution  = { hmdInfo.Resolution.w, hmdInfo.Resolution.h };
        m_description.device      = Device::OculusRift;

        for (auto eye : { LeftEye, RightEye })
            m_description.idealResolution[eye] = { m_eyeRenderViewport[eye].Size.w, m_eyeRenderViewport[eye].Size.h };
    }

    //----------------------------------------------------------------------
    bool OculusRift::_HasFocus()
    {
        ovrSessionStatus sessionStatus;
        ovr_GetSessionStatus( g_session, &sessionStatus );
        return sessionStatus.HasInputFocus;
    }

} } // End namespaces