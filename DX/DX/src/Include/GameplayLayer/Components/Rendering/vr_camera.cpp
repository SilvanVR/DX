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
#include "mesh_renderer.h"
#include "Events/event_dispatcher.h"


namespace Components {

    #define BUFFER_FORMAT_LDR       Graphics::TextureFormat::RGBA32
    #define BUFFER_FORMAT_HDR       Graphics::TextureFormat::RGBAFloat
    #define DEPTH_STENCIL_FORMAT    Graphics::TextureFormat::D32

    using namespace Graphics::VR;

    //----------------------------------------------------------------------
    VRCamera::VRCamera( ScreenDisplay screenDisplay, Graphics::MSAASamples sampleCount, bool hdr )
    {
        ASSERT( RENDERER.hasHMD() && "Component requires an HMD but there is none available!" );

        // Create the cameras in the constructor, so other scripts can access the cameras early on (e.g. for postprocessing)
        auto& hmd = RENDERER.getHMD();
        auto hmdDesc = hmd.getDescription();
        for (auto eye : { LeftEye, RightEye })
        {
            m_eyeGameObjects[eye] = THIS_SCENE.createGameObject( eye == 0 ? "LeftEye" : "RightEye" );

            // Create cameras
            auto eyeBuffer = RESOURCES.createRenderTexture( hmdDesc.idealResolution[eye].x, hmdDesc.idealResolution[eye].y,
                                                            DEPTH_STENCIL_FORMAT, hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR,
                                                            sampleCount, false );
            m_eyeCameras[eye] = m_eyeGameObjects[eye]->addComponent<Components::Camera>( eyeBuffer );
            m_eyeCameras[eye]->setProjection( hmd.getProjection( eye, m_eyeCameras[eye]->getZNear(), m_eyeCameras[eye]->getZFar() ) );
        }

        // Update transform data before every frame
        m_frameBeginListener = Events::EventDispatcher::GetEvent( EVENT_FRAME_BEGIN ).addListener( BIND_THIS_FUNC_0_ARGS( &VRCamera::_OnFrameBegin ) );

        // Set which camera renders to screen
        setScreenDisplay( screenDisplay );
    }

    //----------------------------------------------------------------------
    void VRCamera::addedToGameObject( GameObject* go )
    {
        // Attach both cameras as childs to this transform.
        // Must be done here because the component is not yet attached to a gameobject in the constructor.
        for (auto eye : { LeftEye, RightEye })
            m_eyeGameObjects[eye]->getTransform()->setParent( go->getTransform() );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void VRCamera::setActive( bool active )
    {
        IComponent::setActive( active );
        for (auto eye : { LeftEye, RightEye })
            m_eyeGameObjects[eye]->setActive( active );
    }

    //----------------------------------------------------------------------
    Transform* VRCamera::getHeadTransform()
    { 
        return m_eyeGameObjects[0]->getTransform(); 
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
    void VRCamera::setClearColor( Color color )
    {
        for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
            m_eyeCameras[eye]->setClearColor( color );
    }

    //----------------------------------------------------------------------
    void VRCamera::setWorldScale( F32 newWorldScale )
    {
        RENDERER.getHMD().setWorldScale( newWorldScale );
    }

    //----------------------------------------------------------------------
    F32 VRCamera::getWorldScale() const
    {
        return RENDERER.getHMD().getWorldScale();
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void VRCamera::_OnFrameBegin()
    {
        auto& hmd = RENDERER.getHMD();
        auto eyePoses = hmd.getEyePoses( Locator::getCoreEngine().getFrameCount() );
        for (auto eye : { LeftEye, RightEye })
        {
            auto transform = m_eyeGameObjects[eye]->getTransform();
            transform->position = eyePoses[eye].position;
            transform->rotation = eyePoses[eye].rotation;
        }
    }

    //**********************************************************************
    // VRTouch
    //**********************************************************************

    //----------------------------------------------------------------------
    VRTouch::VRTouch( Graphics::VR::Hand hand )
        : m_hand( hand )
    {
        // Update transform data before every frame
        m_frameBeginListener = Events::EventDispatcher::GetEvent( EVENT_FRAME_BEGIN ).addListener( [this]() {
            auto& touchPose = RENDERER.getHMD().getTouchPose( m_hand, Locator::getCoreEngine().getFrameCount() );

            auto transform = getGameObject()->getTransform();
            transform->position = touchPose.position;
            transform->rotation = touchPose.rotation;
        });
    }

    //**********************************************************************
    // VRBasicTouch
    //**********************************************************************

    //----------------------------------------------------------------------
    VRBasicTouch::VRBasicTouch( const MeshPtr& mesh, const MaterialPtr& material )
    {
        for (auto hand : { Hand::Left, Hand::Right })
        {
            m_handGameObject[(I32)hand] = THIS_SCENE.createGameObject( hand == Hand::Left ? "LeftHand" : "RightHand" );
            m_handGameObject[(I32)hand]->addComponent<Components::VRTouch>( hand );
            m_handGameObject[(I32)hand]->addComponent<Components::MeshRenderer>( mesh, material );
        }

        m_hmdFocusGainedListener = Events::EventDispatcher::GetEvent( EVENT_HMD_FOCUS_GAINED ).addListener([this]{
            for (auto hand : { Hand::Left, Hand::Right })
                m_handGameObject[(I32)hand]->setActive( true );
        });
        m_hmdFocusLostListener = Events::EventDispatcher::GetEvent( EVENT_HMD_FOCUS_LOST ).addListener([this] {
            for (auto hand : { Hand::Left, Hand::Right })
                m_handGameObject[(I32)hand]->setActive( false );
        });
    }

    //----------------------------------------------------------------------
    VRBasicTouch::VRBasicTouch( const MeshPtr& leftHandMesh, const MeshPtr& rightHandMesh, const MaterialPtr& material )
    {
        for (auto hand : { Hand::Left, Hand::Right })
        {
            m_handGameObject[(I32)hand] = THIS_SCENE.createGameObject( hand == Hand::Left ? "LeftHand" : "RightHand" );
            m_handGameObject[(I32)hand]->addComponent<Components::VRTouch>( hand );
            m_handGameObject[(I32)hand]->addComponent<Components::MeshRenderer>( hand == Hand::Left ? leftHandMesh : rightHandMesh, material );
        }
    }

    //----------------------------------------------------------------------
    void VRBasicTouch::addedToGameObject( GameObject* go )
    {
        for (auto hand : { Hand::Left, Hand::Right })
            m_handGameObject[(I32)hand]->getTransform()->setParent( go->getTransform(), false );
    }

}