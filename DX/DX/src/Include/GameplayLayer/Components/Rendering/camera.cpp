#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "Core/locator.h"
#include "GameplayLayer/gameobject.h"

namespace Components {

    #define BUFFER_FORMAT_LDR       Graphics::TextureFormat::RGBA32
    #define BUFFER_FORMAT_HDR       Graphics::TextureFormat::RGBAFloat
    #define DEPTH_STENCIL_FORMAT    Graphics::TextureFormat::D32

    //----------------------------------------------------------------------
    Camera::Camera( const RenderTexturePtr& rt, F32 fovAngleYInDegree, F32 zNear, F32 zFar )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
        setRenderTarget( rt, Graphics::CameraFlags::BlitToScreen );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples, bool hdr )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
        _CreateRenderTarget( numSamples, hdr );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples, bool hdr )
        : m_camera( left, right, bottom, top, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
        _CreateRenderTarget( numSamples, hdr );
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
            pair.second.erase( std::remove(pair.second.begin(), pair.second.end(), cmd ), pair.second.end() );
    }

    //----------------------------------------------------------------------
    void Camera::setHDRRendering( bool enabled )
    {
        bool hdr = isHDR();
        if (hdr == enabled)
            return;

        getRenderTarget()->recreate( hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR );
    }

    //----------------------------------------------------------------------
    void Camera::setSuperSampling( F32 screenResMod )
    {
        if (getRenderTarget()->getDynamicScaleFactor() == screenResMod)
            return;
        getRenderTarget()->setDynamicScreenScale( true, screenResMod );
    }

    bool Camera::isHDR() const 
    { 
        return m_camera.getRenderTarget()->getFormat() == BUFFER_FORMAT_HDR;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::_CreateRenderTarget( Graphics::MSAASamples sampleCount, bool hdr )
    {
        auto& window = Locator::getWindow();
        auto rt = RESOURCES.createRenderTexture( window.getWidth(), window.getHeight(), 
                                                 DEPTH_STENCIL_FORMAT, hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR,
                                                 sampleCount, true );
        setRenderTarget( rt, Graphics::CameraFlags::BlitToScreen );
    }

}