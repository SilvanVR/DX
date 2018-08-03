#include "fps_camera.h"
/**********************************************************************
    class: FPSCamera

    author: S. Hau
    date: March 7, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "transform.h"
#include "Core/locator.h"
#include "Math/math_utils.h"
#include "Rendering/vr_camera.h"

namespace Components {

    //----------------------------------------------------------------------
    void FPSCamera::addedToGameObject( GameObject* go )
    {
        m_pTransform = go->getComponent<Components::Transform>();

        // Save start position by setting necessary fields, otherwise the script will lerp to the default values
        m_desiredDistance = m_pointOfInterest.distance( m_pTransform->position );
        m_pTransform->lookAt( m_pointOfInterest );
        _ResetAnglesToCurrentView();
    }

    //----------------------------------------------------------------------
    void FPSCamera::onActive()
    {
        _ResetAnglesToCurrentView();
    }

    //----------------------------------------------------------------------
    void FPSCamera::lateTick( Time::Seconds delta )
    {
        if ( MOUSE.wasKeyPressed( MouseKey::MButton ) )
        {
            m_cameraMode = (m_cameraMode == ECameraMode::MAYA ? ECameraMode::FPS : ECameraMode::MAYA);

            switch (m_cameraMode)
            {
            case ECameraMode::FPS: 
                break;
            case ECameraMode::MAYA:
                m_desiredDistance = m_pointOfInterest.distance( m_pTransform->position );
                m_pTransform->lookAt( m_pointOfInterest );
                _ResetAnglesToCurrentView();
                break;
            }
        }

        if( KEYBOARD.isKeyDown( Key::R ) )
        {
            auto lookToCenter = Math::Quat::LookRotation( (m_pointOfInterest - m_pTransform->position), Math::Vec3::UP );
            auto eulers = lookToCenter.toEulerAngles();
            m_mousePitchDeg = eulers.x;
            m_mouseYawDeg = eulers.y;
        }

        switch (m_cameraMode)
        {
        case ECameraMode::FPS:  _UpdateFPSCamera( (F32)delta ); break;
        case ECameraMode::MAYA: _UpdateMayaCamera( (F32)delta ); break;
        }
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void FPSCamera::_UpdateFPSCamera( F32 delta )
    {
        Math::Vec3 forward  = m_pTransform->rotation.getForward();
        Math::Vec3 left     = m_pTransform->rotation.getLeft();
        Math::Vec3 up       = m_pTransform->rotation.getUp();

        F32 speed = m_fpsSpeed;
        if ( KEYBOARD.isKeyDown( Key::Control ) )
            speed *= 10.0f;

        m_pTransform->position += left    * (F32)AXIS_MAPPER.getAxisValue( "Horizontal" ) * speed;
        m_pTransform->position += forward * (F32)AXIS_MAPPER.getAxisValue( "Vertical" )   * speed;
        m_pTransform->position += up      * (F32)AXIS_MAPPER.getAxisValue( "Up" )         * speed;

        // Rotation with mouse using delta-mouse
        if ( MOUSE.isKeyDown( MouseKey::RButton ) )
        {
            auto deltaMouse = MOUSE.getMouseDelta();
            m_mousePitchDeg += deltaMouse.y * m_mouseSensitivity;
            m_mouseYawDeg += deltaMouse.x * m_mouseSensitivity;
        } 

        // Smoothly lerp to desired rotation
        Math::Quat desiredRotation = Math::Quat::FromEulerAngles( m_mousePitchDeg, m_mouseYawDeg, 0.0f );
        m_pTransform->rotation = Math::Quat::Slerp( m_pTransform->rotation, desiredRotation, 0.1f * m_mouseDamping );

        // Scroll wheel
        m_pTransform->position += m_pTransform->rotation.getForward() * (F32)AXIS_MAPPER.getMouseWheelAxisValue() * 0.5f;
    }

    //----------------------------------------------------------------------
    void FPSCamera::_UpdateMayaCamera( F32 delta )
    {
        // Standard camera-rotation using delta-mouse
        if ( MOUSE.isKeyDown( MouseKey::RButton ) )
        {
            auto deltaMouse = MOUSE.getMouseDelta();
            m_mousePitchDeg += deltaMouse.y * m_mouseSensitivity * 2.0f;
            m_mouseYawDeg   += deltaMouse.x * m_mouseSensitivity * 2.0f;
        }

        // Adjust distance if wheel is used. Move faster the farther away from POI
        F32 distanceToPOI = (m_pTransform->position - m_pointOfInterest).magnitude();
        m_desiredDistance -= (F32)AXIS_MAPPER.getMouseWheelAxisValue() * distanceToPOI * 0.05f;
        m_desiredDistance = m_desiredDistance < 0.1f ? 0.1f : m_desiredDistance;

        // Calculate quaternion from angles and set the desired position
        auto quat = Math::Quat::FromEulerAngles( m_mousePitchDeg, m_mouseYawDeg, 0.0f );
        Math::Vec3 desiredPosition = quat.getForward() * -m_desiredDistance;

        // Smoothly lerp to target position
        m_pTransform->position = Math::Lerp( m_pTransform->position, desiredPosition, 0.2f * m_mouseDamping );

        // Make sure we are always looking at the target
        m_pTransform->lookAt( m_pointOfInterest );
    }

    //----------------------------------------------------------------------
    void FPSCamera::_ResetAnglesToCurrentView()
    {
        auto eulers = m_pTransform->rotation.toEulerAngles();
        m_mousePitchDeg = eulers.x;
        m_mouseYawDeg   = eulers.y;
    }

    //**********************************************************************
    // VRFPSCamera
    //**********************************************************************

    //----------------------------------------------------------------------
    void VRFPSCamera::init()
    {
        m_vrCamera = getGameObject()->getComponent<VRCamera>();
        ASSERT( m_vrCamera && "Script requires a vr camera component!" );
    }

    //----------------------------------------------------------------------
    void VRFPSCamera::tick( Time::Seconds d )
    {
        auto delta = (F32)d;
        F32 speed = m_speed;
        if (KEYBOARD.isKeyDown(Key::Shift))
            speed *= 5.0f;

        // Move in look direction
        auto transform = getGameObject()->getTransform();
        Math::Vec3 lookDir = m_vrCamera->getLookDirection();
        if (KEYBOARD.isKeyDown(Key::W)) transform->position += lookDir * speed * delta;
        if (KEYBOARD.isKeyDown(Key::S)) transform->position -= lookDir * speed * delta;
        transform->position += lookDir * (F32)AXIS_MAPPER.getMouseWheelAxisValue() * 0.3f;

        // Rotate around y-axis in fixed steps
        if (KEYBOARD.wasKeyPressed(Key::A)) transform->rotation *= Math::Quat({0, 1, 0}, -m_rotationAngle);
        if (KEYBOARD.wasKeyPressed(Key::D)) transform->rotation *= Math::Quat({0, 1, 0}, m_rotationAngle);
    }


}