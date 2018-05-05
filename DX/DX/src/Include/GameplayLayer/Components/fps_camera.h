#pragma once
/**********************************************************************
    class: FPSCamera (fps_camera.h)

    author: S. Hau
    date: March 7, 2018

    Script which controls the transform of the attached gameobject via
    Axis-Input (Usually Mouse/Keyboard):
      > 2 Modes:
        FPS: First-Person Shooter Style:
          Move with WASD and look around with mouse.
        Maya: Maya Observation Mode:
          Camera looks always at the center. Rotation always around it.
    In both modes, moving forward/backwards is possible with the scroll-wheel.
**********************************************************************/

#include "i_component.h"

namespace Components {

    class Transform;

    //**********************************************************************
    class FPSCamera : public Components::IComponent
    {
    public:
        enum ECameraMode
        {
            FPS,
            MAYA
        };

        //----------------------------------------------------------------------
        // Constructor for the FPS-Mode.
        // @Params:
        // "speed": How fast the camera moves.
        // "mouse-sensitivity": Determines how fast the rotation will be
        // "damping": How fast the mouse rotation slows down
        //----------------------------------------------------------------------
        FPSCamera( ECameraMode cameraMode = ECameraMode::MAYA, F32 fpsSpeed = 0.6f, F32 mouseSensitivity = 0.3f, F32 mouseDamping = 1.0f ) 
            : m_fpsSpeed( fpsSpeed ), m_mouseSensitivity( mouseSensitivity * 0.5f ), m_cameraMode( cameraMode ), m_mouseDamping( mouseDamping ) {}

        //----------------------------------------------------------------------
        void setCameraMode(ECameraMode newMode){ m_cameraMode = newMode; }

    private:
        Components::Transform*  m_pTransform            = nullptr;
        Math::Vec3              m_pointOfInterest       = Math::Vec3(0,0,0);
        F32                     m_fpsSpeed              = 1.0f;
        F32                     m_mouseSensitivity      = 1.0f;
        ECameraMode             m_cameraMode            = ECameraMode::FPS;

        F32                     m_mousePitchDeg         = 0;
        F32                     m_mouseYawDeg           = 0;
        F32                     m_mouseDamping          = 1.0f;
        F32                     m_desiredDistance       = 10.0f;


        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void lateTick(Time::Seconds delta) override;
        void onActive() override;

        //----------------------------------------------------------------------
        void _UpdateFPSCamera(F32 delta);
        void _UpdateMayaCamera(F32 delta);

        inline void _ResetAnglesToCurrentView();

        //----------------------------------------------------------------------
        FPSCamera(const FPSCamera& other)               = delete;
        FPSCamera& operator = (const FPSCamera& other)  = delete;
        FPSCamera(FPSCamera&& other)                    = delete;
        FPSCamera& operator = (FPSCamera&& other)       = delete;
    };

}