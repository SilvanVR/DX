#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "Core/locator.h"

namespace Components {

    #define BUFFER_FORMAT_LDR       Graphics::TextureFormat::RGBA32
    #define BUFFER_FORMAT_HDR       Graphics::TextureFormat::RGBAFloat
    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32

    //----------------------------------------------------------------------
    Camera::Camera( const RenderTexturePtr& rt, F32 fovAngleYInDegree, F32 zNear, F32 zFar )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
        setRenderTarget( rt, true );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples, bool hdr )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL ), m_hdr( hdr )
    {
        _CreateRenderTarget( numSamples );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples, bool hdr )
        : m_camera( left, right, bottom, top, zNear, zFar ), m_cullingMask( LAYER_ALL ), m_hdr( hdr )
    {
        _CreateRenderTarget( numSamples );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::addCommandBuffer( Graphics::CommandBuffer* cmd, CameraEvent evt )
    { 
        m_additionalCommandBuffers[evt].push_back( cmd );
    }

    //----------------------------------------------------------------------
    void Camera::removeCommandBuffer( Graphics::CommandBuffer* cmd ) 
    { 
        for (auto& pair : m_additionalCommandBuffers)
            pair.second.erase( std::remove( pair.second.begin(), pair.second.end(), cmd ) );
    }

    //----------------------------------------------------------------------
    void Camera::setHDRRendering( bool enabled )
    {
        if (m_hdr == enabled)
            return;

        m_hdr = enabled;
        getRenderTarget()->recreate( m_hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR );
    }

    //----------------------------------------------------------------------
    void Camera::setSuperSampling( F32 screenResMod )
    {
        if (getRenderTarget()->getDynamicScaleFactor() == screenResMod)
            return;
        getRenderTarget()->setDynamicScreenScale( true, screenResMod );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::_CreateRenderTarget( Graphics::MSAASamples sampleCount )
    {
        auto& window = Locator::getWindow();
        auto rt = RESOURCES.createRenderTexture( window.getWidth(), window.getHeight(), 
                                                 DEPTH_STENCIL_FORMAT, m_hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR, 
                                                 sampleCount, true );
        rt->setDynamicScreenScale( true, 1.0f );

        setRenderTarget( rt, true );
    }

}