#include "fps_camera.h"
/**********************************************************************
    class: FPSCamera (fps_camera.h)

    author: S. Hau
    date: March 7, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "transform.h"
#include "locator.h"

namespace Components {

    //----------------------------------------------------------------------
    void FPSCamera::addedToGameObject( GameObject* go )
    {
        m_pTransform = go->getComponent<Components::Transform>();
    }

    //----------------------------------------------------------------------
    void FPSCamera::Tick( Time::Seconds delta )
    {
        if ( MOUSE.wasKeyPressed( MouseKey::MButton ) )
            m_cameraMode = (m_cameraMode == ECameraMode::MAYA ? ECameraMode::FPS : ECameraMode::MAYA);

        switch (m_cameraMode)
        {
        case ECameraMode::FPS:  _UpdateFPSCamera( (F32)delta.value ); break;
        case ECameraMode::MAYA: _UpdateMayaCamera( (F32)delta.value ); break;
        }

        if( KEYBOARD.isKeyDown( Key::R ) )
            m_pTransform->lookAt( Math::Vec3(0) );
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

        m_pTransform->position += left    * (F32)AXIS_MAPPER.getAxisValue( "Horizontal" ) * delta * m_fpsSpeed;
        m_pTransform->position += forward * (F32)AXIS_MAPPER.getAxisValue( "Vertical" )   * delta * m_fpsSpeed;
        m_pTransform->position += up      * (F32)AXIS_MAPPER.getAxisValue( "Up" )         * delta * m_fpsSpeed;

        // Rotation with mouse using delta-mouse
        if ( MOUSE.isKeyDown( MouseKey::RButton ) )
        {
            auto deltaMouse = MOUSE.getMouseDelta();
            m_pTransform->rotation *= Math::Quat(m_pTransform->rotation.getRight(), deltaMouse.y * m_fpsMouseSensitivity);
            m_pTransform->rotation *= Math::Quat(Math::Vec3::UP, deltaMouse.x * m_fpsMouseSensitivity);
        } 

        m_pTransform->position += m_pTransform->rotation.getForward() * (F32)AXIS_MAPPER.getMouseWheelAxisValue() * delta * 20.0f;
    }

    //----------------------------------------------------------------------
    void FPSCamera::_UpdateMayaCamera( F32 delta )
    {
        float distanceToPOI = (m_pTransform->position - m_pointOfInterest).magnitude();

        // standard camera-rotation using delta-mouse
        if ( MOUSE.isKeyDown( MouseKey::RButton ) )
        {
            auto deltaMouse = MOUSE.getMouseDelta();

            // The further away the faster the rotation
            F32 amt = distanceToPOI * 0.005f;
            m_pTransform->position += m_pTransform->rotation.getUp() * (F32)deltaMouse.y * amt;
            m_pTransform->position += m_pTransform->rotation.getLeft() * (F32)deltaMouse.x * amt;

            // Adjust rotation
            m_pTransform->lookAt( m_pointOfInterest );

            // Adjust distance
            m_pTransform->position = m_pointOfInterest - (m_pTransform->rotation.getForward() * distanceToPOI);
        }

        m_pTransform->position += m_pTransform->rotation.getForward() * (F32)AXIS_MAPPER.getMouseWheelAxisValue() * delta * distanceToPOI * 2.0f;
    }


}