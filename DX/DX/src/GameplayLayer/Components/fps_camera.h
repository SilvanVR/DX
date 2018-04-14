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
        // "speed": How fast the object moves.
        // "mouse-sensitivity": Determines how fast the rotation will be.
        //----------------------------------------------------------------------
        FPSCamera( ECameraMode cameraMode = ECameraMode::MAYA, F32 fpsSpeed = 10.0f, F32 fpsMouseSensitivity = 0.5f ) 
            : m_fpsSpeed( fpsSpeed ), m_fpsMouseSensitivity( fpsMouseSensitivity ), m_cameraMode( cameraMode ) {}

        //----------------------------------------------------------------------
        void setCameraMode(ECameraMode newMode){ m_cameraMode = newMode; }

    private:
        Components::Transform*  m_pTransform            = nullptr;
        Math::Vec3              m_pointOfInterest       = Math::Vec3(0,0,0);
        F32                     m_fpsSpeed              = 1.0f;
        F32                     m_fpsMouseSensitivity   = 1.0f;
        ECameraMode             m_cameraMode            = ECameraMode::FPS;

        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void tick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        void _UpdateFPSCamera(F32 delta);
        void _UpdateMayaCamera(F32 delta);

        //----------------------------------------------------------------------
        FPSCamera(const FPSCamera& other)               = delete;
        FPSCamera& operator = (const FPSCamera& other)  = delete;
        FPSCamera(FPSCamera&& other)                    = delete;
        FPSCamera& operator = (FPSCamera&& other)       = delete;
    };

}