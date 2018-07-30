#include "oculus_rift.h"
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    OculusRift::OculusRift( API api )
    {
        if ( not _InitLibOVR() )
            return;

        m_HMDInfo = ovr_GetHmdDesc( m_session );
        _CreateEyeBuffers( api, m_HMDInfo );

        for (auto eye : { left, right })
            m_eyeRenderDesc[eye] = ovr_GetRenderDesc( m_session, (ovrEyeType)eye, m_HMDInfo.DefaultEyeFov[eye] );

        m_initialized = true;
    }

    //----------------------------------------------------------------------
    OculusRift::~OculusRift()
    {
        if (m_session)
        {
            for (auto eye : { left, right })
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
    std::array<DirectX::XMMATRIX, 2> OculusRift::getEyeMatrices( I64 frameIndex )
    {
        ASSERT( m_session && "VR Session does not exist." );
        std::array<DirectX::XMMATRIX, 2> matrices{};

        ovrPosef HmdToEyePose[2] = { m_eyeRenderDesc[0].HmdToEyePose, m_eyeRenderDesc[1].HmdToEyePose };
        F64 ftiming = ovr_GetPredictedDisplayTime( m_session, frameIndex );
        ovrTrackingState hmdState = ovr_GetTrackingState( m_session, ftiming, ovrTrue );
        ovr_CalcEyePoses( hmdState.HeadPose.ThePose, HmdToEyePose, m_currentEyeRenderPose );

        using namespace DirectX;
        for (auto eye : { left, right })
        {
            XMVECTOR eyeQuat = XMVectorSet( m_currentEyeRenderPose[eye].Orientation.x, m_currentEyeRenderPose[eye].Orientation.y,
                                            m_currentEyeRenderPose[eye].Orientation.z, m_currentEyeRenderPose[eye].Orientation.w );
            XMVECTOR eyePos = XMVectorSet( m_currentEyeRenderPose[eye].Position.x, m_currentEyeRenderPose[eye].Position.y, m_currentEyeRenderPose[eye].Position.z, 0 );
            //XMVECTOR CombinedPos = XMVectorAdd(mainCam.Pos, XMVector3Rotate(eyePos, mainCam.Rot));
            //Camera finalCam(CombinedPos, (XMQuaternionMultiply(eyeQuat, mainCam.Rot)));
            //XMMATRIX view = finalCam.GetViewMatrix();
            ovrMatrix4f p = ovrMatrix4f_Projection( m_eyeRenderDesc[eye].Fov, 0.2f, 1000.0f, ovrProjection_None );
            XMMATRIX proj = XMMatrixSet( p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
                                         p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
                                         p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
                                         p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3] );
        }

        return matrices;
    }

    //----------------------------------------------------------------------
    void OculusRift::distortAndPresent( I64 frameIndex )
    {
        ASSERT( m_session && "VR Session does not exist." );
        ovrLayerEyeFov ld;
        ld.Header.Type = ovrLayerType_EyeFov;
        ld.Header.Flags = 0;
        for (auto eye : { left, right })
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
        m_isVisible = ovr_SubmitFrame( m_session, frameIndex, &viewScaleDesc, &layers, 1 ) == ovrSuccess;
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    bool OculusRift::_InitLibOVR()
    {
        ovrResult result = ovr_Initialize( nullptr );
        if (result != ovrSuccess)
        {
            LOG_WARN_RENDERING( "OculusRift: Failed to initialize libOVR. It may not be supported on your system." );
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
        for (auto eye : { left, right })
        {
            ovrSizei idealSize = ovr_GetFovTextureSize( m_session, (ovrEyeType)eye, HMDInfo.DefaultEyeFov[eye], 1.0f );
            m_eyeRenderViewport[eye].Pos.x = 0;
            m_eyeRenderViewport[eye].Pos.y = 0;
            m_eyeRenderViewport[eye].Size = idealSize;

            m_eyeBuffers[eye] = new OculusSwapchain( api, m_session, idealSize.w, idealSize.h );
        }
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