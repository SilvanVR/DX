#include "audio_listener.h"
/**********************************************************************
    class: AudioListener (audio_listener.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "transform.h"
#include "GameplayLayer/gameobject.h"
#include "Core/locator.h"
#include "Rendering/vr_camera.h"
#include "Rendering/camera.h"

namespace Components {

    //----------------------------------------------------------------------
    void AudioListener::addedToGameObject( GameObject* go )
    {
        _UpdateListener();
    }

    //----------------------------------------------------------------------
    void AudioListener::tick( Time::Seconds delta )
    {
        _UpdateListener();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void AudioListener::_UpdateListener()
    {
        Components::Transform* transform;
        if ( auto vrCam = getGameObject()->getComponent<VRCamera>() )
            transform = vrCam->getCameraForEye(Graphics::VR::LeftEye).getGameObject()->getTransform();
        else
            transform = getGameObject()->getTransform();

        X3DAUDIO_LISTENER listener = {};
        auto worldRotation = transform->getWorldRotation();
        listener.OrientFront    = worldRotation.getForward();
        listener.OrientTop      = worldRotation.getUp();
        listener.Position       = transform->getWorldPosition();

        Locator::getAudioManager().updateListener( listener );
    }

}