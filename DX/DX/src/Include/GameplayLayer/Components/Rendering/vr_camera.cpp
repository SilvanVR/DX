#include "vr_camera.h"
/**********************************************************************
    class: VRCamera

    author: S. Hau
    date: August 1, 2018
**********************************************************************/

#include "Core/locator.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "Graphics/VR/vr.h"
#include "camera.h"

#include "Events/event_dispatcher.h"
#include "Events/event_names.hpp"

namespace Components
{
    #define BUFFER_FORMAT_LDR       Graphics::TextureFormat::RGBA32
    #define BUFFER_FORMAT_HDR       Graphics::TextureFormat::RGBAFloat
    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32

    //----------------------------------------------------------------------
    VRCamera::VRCamera( ScreenDisplay screenDisplay, Graphics::MSAASamples sampleCount, bool hdr )
        : m_sampleCount( sampleCount ), m_hdr( hdr ), m_screenDisplay( screenDisplay )
    {
        // Create the cameras in the constructor, so other scripts can access the cameras early on (e.g. for postprocessing)

        // Create eye gameobjects and cameras
        auto hmdDesc = RENDERER.getVRDevice().getDescription();
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
        {
            m_eyeGameObjects[eye] = THIS_SCENE.createGameObject( eye == 0 ? "LeftEye" : "RightEye" );

            // Create cameras
            auto eyeBuffer = RESOURCES.createRenderTexture( hmdDesc.idealResolution[eye].x, hmdDesc.idealResolution[eye].y,
                                                            DEPTH_STENCIL_FORMAT, m_hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR,
                                                            m_sampleCount, false );
            m_eyeCameras[eye] = m_eyeGameObjects[eye]->addComponent<Components::Camera>( eyeBuffer );
        }

        // Set which camera renders to screen
        setScreenDisplay( m_screenDisplay );

        m_frameEvtListenerID = Events::EventDispatcher::GetEvent( EVENT_FRAME_BEGIN ).addListener( BIND_THIS_FUNC_0_ARGS( &VRCamera::_FrameBegin ) );
    }

    //----------------------------------------------------------------------
    void VRCamera::addedToGameObject( GameObject* go )
    {
        // Attach both cameras as childs to this transform.
        // Must be done here because the component is not yet attached to a gameobject in the constructor.
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
            m_eyeGameObjects[eye]->getTransform()->setParent( go->getTransform() );
    }

    //----------------------------------------------------------------------
    void VRCamera::shutdown()
    {
        Events::EventDispatcher::GetEvent( EVENT_FRAME_BEGIN ).removeListener( m_frameEvtListenerID );
        auto scene = getGameObject()->getScene();
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
            scene->destroyGameObject( m_eyeGameObjects[eye] );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void VRCamera::setActive( bool active )
    {
        IComponent::setActive( active );
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
            m_eyeGameObjects[eye]->setActive( active );
    }

    //----------------------------------------------------------------------
    Math::Vec3 VRCamera::getLookDirection()
    { 
        return m_eyeGameObjects[0]->getTransform()->getWorldRotation().getForward(); 
    }

    //----------------------------------------------------------------------
    void VRCamera::setScreenDisplay( ScreenDisplay sd )
    {
        // First reset viewport
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
        {
            auto &vp = m_eyeCameras[eye]->getViewport();
            vp.height = vp.width = 1.0f;
            vp.topLeftX = vp.topLeftY = 0.0f;
        }

        switch (sd)
        {
        case ScreenDisplay::None:
            m_eyeCameras[Graphics::VR::LeftEye]->setCameraFlags( Graphics::CameraFlags::BlitToLeftEye );
            m_eyeCameras[Graphics::VR::RightEye]->setCameraFlags( Graphics::CameraFlags::BlitToRightEye );
            break;
        case ScreenDisplay::LeftEye: 
            m_eyeCameras[Graphics::VR::LeftEye]->setCameraFlags( Graphics::CameraFlags::BlitToScreenAndLeftEye );
            m_eyeCameras[Graphics::VR::RightEye]->setCameraFlags( Graphics::CameraFlags::BlitToRightEye );
            break;
        case ScreenDisplay::RightEye: 
            m_eyeCameras[Graphics::VR::LeftEye]->setCameraFlags( Graphics::CameraFlags::BlitToLeftEye );
            m_eyeCameras[Graphics::VR::RightEye]->setCameraFlags( Graphics::CameraFlags::BlitToScreenAndRightEye );
            break;
        case ScreenDisplay::BothEyes:
            m_eyeCameras[0]->getViewport().width = 0.5f;
            m_eyeCameras[1]->getViewport().topLeftX = 0.5f;
            m_eyeCameras[1]->getViewport().width = 0.5f;
            m_eyeCameras[Graphics::VR::LeftEye]->setCameraFlags( Graphics::CameraFlags::BlitToScreenAndLeftEye );
            m_eyeCameras[Graphics::VR::RightEye]->setCameraFlags( Graphics::CameraFlags::BlitToScreenAndRightEye );
        }
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void VRCamera::_FrameBegin()
    {
        auto eyePoses = RENDERER.getVRDevice().calculateEyePoses(0);
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
        {
            auto transform = m_eyeGameObjects[eye]->getTransform();
            transform->position = eyePoses[eye].position;
            transform->rotation = eyePoses[eye].rotation;
            m_eyeCameras[eye]->setProjection( eyePoses[eye].projection );
        }
    }


}