#include "audio_listener.h"
/**********************************************************************
    class: AudioListener (audio_listener.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "transform.h"
#include "GameplayLayer/gameobject.h"
#include "locator.h"

namespace Components {

    //----------------------------------------------------------------------
    void AudioListener::AddedToGameObject( GameObject* go )
    {
        _UpdateListener();
    }

    //----------------------------------------------------------------------
    void AudioListener::Tick( Time::Seconds delta )
    {
        _UpdateListener();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void AudioListener::_UpdateListener()
    {
        auto transform = getComponent<Components::Transform>();

        X3DAUDIO_LISTENER listener = {};
        listener.OrientFront    = transform->rotation.getForward();
        listener.OrientTop      = transform->rotation.getUp();
        listener.Position       = transform->position;

        Locator::getAudioManager().updateListener( listener );
    }

}