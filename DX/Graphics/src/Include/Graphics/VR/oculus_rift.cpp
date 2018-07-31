#include "oculus_rift.h"
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "Logging/logging.h"

//----------------------------------------------------------------------
inline Math::Vec3 ConvertVec3( const ovrVector3f& vec3 )
{
    return Math::Vec3( vec3.x, vec3.y, vec3.z );
}

//----------------------------------------------------------------------
inline Math::Quat ConvertQuat( const ovrQuatf& q )
{
    return Math::Quat( q.x, q.y, q.z, q.w );
}


namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    OculusRift::OculusRift( API api )
    {
        if ( not _InitLibOVR() )
            return;

        m_HMDInfo = ovr_GetHmdDesc( m_session );
        _CreateEyeBuffers( api, m_HMDInfo );

        for (auto eye : { Left, Right })
            m_eyeRenderDesc[eye] = ovr_GetRenderDesc( m_session, (ovrEyeType)eye, m_HMDInfo.DefaultEyeFov[eye] );

        _SetupDescription( m_HMDInfo );
        m_initialized = true;
    }

    //----------------------------------------------------------------------
    OculusRift::~OculusRift()
    {
        if (m_session)
        {
            for (auto eye : { Left, Right })
            {
                m_eyeBuffers[eye]->release( m_session );
                delete m_eyeBuffers[eye];
            }
            ovr_Destroy( m_session );
            ovr_Shutdown();
        }
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    std::array<EyePose, 2> OculusRift::getEyePoses() const
    {
        std::array<EyePose, 2> eyePoses{};
        for (auto eye : { Left, Right })
        {
            eyePoses[eye].position = ConvertVec3( m_currentEyeRenderPose->Position );
            eyePoses[eye].rotation = ConvertQuat( m_currentEyeRenderPose->Orientation );
        }
        return eyePoses;
    }

    //----------------------------------------------------------------------
    std::array<EyePose, 2> OculusRift::calculateEyePoses( I64 frameIndex ) 
    {
        std::array<EyePose, 2> eyePoses{};

        ovrPosef HmdToEyePose[2] = { m_eyeRenderDesc[0].HmdToEyePose, m_eyeRenderDesc[1].HmdToEyePose };
        F64 ftiming = ovr_GetPredictedDisplayTime( m_session, frameIndex );
        ovrTrackingState hmdState = ovr_GetTrackingState( m_session, ftiming, ovrTrue );

        //if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
        ovr_CalcEyePoses( hmdState.HeadPose.ThePose, HmdToEyePose, m_currentEyeRenderPose );

        using namespace DirectX;
        for (auto eye : { Left, Right })
        {
            XMVECTOR eyeQuat = XMVectorSet( m_currentEyeRenderPose[eye].Orientation.x, m_currentEyeRenderPose[eye].Orientation.y,
                                            m_currentEyeRenderPose[eye].Orientation.z, m_currentEyeRenderPose[eye].Orientation.w );
            XMVECTOR eyePos = XMVectorSet( m_currentEyeRenderPose[eye].Position.x, m_currentEyeRenderPose[eye].Position.y, m_currentEyeRenderPose[eye].Position.z, 0 );
            //XMVECTOR CombinedPos = XMVectorAdd(mainCam.Pos, XMVector3Rotate(eyePos, mainCam.Rot));
            //Camera finalCam(CombinedPos, (XMQuaternionMultiply(eyeQuat, mainCam.Rot)));
            //XMMATRIX view = finalCam.GetViewMatrix();
            ovrMatrix4f p = ovrMatrix4f_Projection( m_eyeRenderDesc[eye].Fov, 0.2f, 1000.0f, ovrProjection_LeftHanded );
            XMMATRIX proj = XMMatrixSet( p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
                                         p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
                                         p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
                                         p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3] );
        }

        for (auto touch : { Hand::Left, Hand::Right })
        {
            m_touch[(I32)touch].position = ConvertVec3( hmdState.HandPoses[(I32)touch].ThePose.Position );
            m_touch[(I32)touch].rotation = ConvertQuat( hmdState.HandPoses[(I32)touch].ThePose.Orientation );
        }

        return eyePoses;
    }

    //----------------------------------------------------------------------
    void OculusRift::distortAndPresent( I64 frameIndex )
    {
        ovrLayerEyeFov ld;
        ld.Header.Type = ovrLayerType_EyeFov;
        ld.Header.Flags = 0;
        for (auto eye : { Left, Right })
        {
            m_eyeBuffers[eye]->commit( m_session );
            ld.ColorTexture[eye]    = m_eyeBuffers[eye]->get();
            ld.Viewport[eye]        = m_eyeRenderViewport[eye];
            ld.Fov[eye]             = m_HMDInfo.DefaultEyeFov[eye];
            ld.RenderPose[eye]      = m_currentEyeRenderPose[eye];
        }

        ovrViewScaleDesc viewScaleDesc;
        viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
        viewScaleDesc.HmdToEyePose[0] = m_eyeRenderDesc[0].HmdToEyePose;
        viewScaleDesc.HmdToEyePose[1] = m_eyeRenderDesc[1].HmdToEyePose;

        ovrLayerHeader* layers = &ld.Header;
        ovrResult result = ovr_SubmitFrame( m_session, frameIndex, &viewScaleDesc, &layers, 1 );

        m_isVisible = result == ovrSuccess;
        if (result == ovrError_DisplayLost)
            LOG_WARN_RENDERING( "OculusRift: HMD was disconnected from the computer." );
    }

    //----------------------------------------------------------------------
    bool OculusRift::hasFocus()
    {
        ovrSessionStatus sessionStatus;
        ovr_GetSessionStatus( m_session, &sessionStatus );
        return sessionStatus.HasInputFocus;
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    bool OculusRift::_InitLibOVR()
    {
        ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
        ovrResult result = ovr_Initialize( &initParams );
        if (result != ovrSuccess)
        {
            LOG_WARN_RENDERING( "OculusRift: Failed to initialize libOVR. It may not be supported/available on your system." );
            return false;
        }

        ovrGraphicsLuid luid;
        result = ovr_Create( &m_session, &luid );
        if (result != ovrSuccess)
        {
            LOG_WARN_RENDERING( "OculusRift: HMD not detected"  );
            return false;
        }

        return true;
    }

    //----------------------------------------------------------------------
    void OculusRift::_CreateEyeBuffers( API api, const ovrHmdDesc& HMDInfo )
    {
        for (auto eye : { Left, Right })
        {
            ovrSizei idealSize = ovr_GetFovTextureSize( m_session, (ovrEyeType)eye, HMDInfo.DefaultEyeFov[eye], 1.0f );
            m_eyeRenderViewport[eye].Pos.x = 0;
            m_eyeRenderViewport[eye].Pos.y = 0;
            m_eyeRenderViewport[eye].Size = idealSize;

            m_eyeBuffers[eye] = new OculusSwapchain( api, m_session, idealSize.w, idealSize.h );
        }
    }

    //----------------------------------------------------------------------
    void OculusRift::_SetupDescription( const ovrHmdDesc& hmdInfo )
    {
        HMDDescription description;
        description.name        = hmdInfo.ProductName;
        description.resolution  = { hmdInfo.Resolution.w, hmdInfo.Resolution.h };
        description.device      = Device::OculusRift;

        for (auto eye : { Left, Right })
        {
            ovrSizei idealSize = ovr_GetFovTextureSize( m_session, (ovrEyeType)eye, hmdInfo.DefaultEyeFov[eye], 1.0f );
            description.idealResolution[eye] = { idealSize.w, idealSize.h };
        }

        m_description = description;
    }

    //**********************************************************************
    // OCULUS SWAPCHAIN
    //**********************************************************************

    //----------------------------------------------------------------------
    OculusSwapchain::OculusSwapchain( API api, ovrSession session, I32 sizeW, I32 sizeH )
    {
        ovrTextureSwapChainDesc dsDesc = {};
        dsDesc.Width = sizeW;
        dsDesc.Height = sizeH;
        dsDesc.MipLevels = 1;
        dsDesc.ArraySize = 1;
        dsDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
        dsDesc.SampleCount = 1;
        dsDesc.MiscFlags = ovrTextureMisc_DX_Typeless;
        dsDesc.BindFlags = ovrTextureBind_DX_RenderTarget;

        switch (api)
        {
        case API::D3D11:
        {
            ovr_CreateTextureSwapChainDX( session, g_pDevice, &dsDesc, &m_swapChain );
            I32 count = 0;
            ovr_GetTextureSwapChainLength( session, m_swapChain, &count );
            for (I32 i = 0; i < count; ++i)
            {
                ID3D11Texture2D* tex = nullptr;
                ovr_GetTextureSwapChainBufferDX( session, m_swapChain, i, IID_PPV_ARGS( &tex ) );
                D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
                rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                g_pDevice->CreateRenderTargetView( tex, &rtvd, &m_texRtv[i].releaseAndGet() );
                tex->Release();
            }
            break;
        }
        case API::Vulkan:
        default: ASSERT( "OculusRift: Unknown graphics api encountered or not supported yet!" );
        }
    }

    //----------------------------------------------------------------------
    void OculusSwapchain::clear( ovrSession session, Color color )
    {
        g_pImmediateContext->ClearRenderTargetView( _GetRTVDX( session ), color.normalized().data() );
    }

} } // End namespaces