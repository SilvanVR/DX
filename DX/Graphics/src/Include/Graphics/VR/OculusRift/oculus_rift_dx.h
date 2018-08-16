#pragma once
/**********************************************************************
    class: OculusRiftDX

    author: S. Hau
    date: August 18, 2018
**********************************************************************/

#include "oculus_rift.h"

// Include API dependant header
#define   OVR_D3D_VERSION 11
#include "LibOVR/OVR_CAPI_D3D.h"
#include "../../D3D11/D3D11.hpp"

namespace Graphics { namespace VR {

    //**********************************************************************
    class OculusSwapchainDX : public OculusSwapchain
    {
    public:
        OculusSwapchainDX(ovrSession session, I32 sizeW, I32 sizeH)
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
        }

        //----------------------------------------------------------------------
        // OculusSwapchain Interface
        //----------------------------------------------------------------------
        void bindForRendering(ovrSession session) override
        {
            auto rtv = _GetRTVDX( session );
            g_pImmediateContext->OMSetRenderTargets( 1, &rtv, NULL );
        }

        void clear(ovrSession session, Color color) override
        {
            g_pImmediateContext->ClearRenderTargetView( _GetRTVDX( session ), color.normalized().data() );
        }

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
    class OculusRiftDX : public OculusRift
    {
    public:
        OculusRiftDX()
        {
            auto desc = getDescription();
            for (auto eye : { LeftEye, RightEye })
                m_eyeBuffers[eye] = new OculusSwapchainDX( getSession(), desc.idealResolution[eye].x, desc.idealResolution[eye].y );
        }
        ~OculusRiftDX() = default;

    private:
        NULL_COPY_AND_ASSIGN(OculusRiftDX)
    };

} } // End namespaces