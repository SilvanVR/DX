#include "audio_source.h"
/**********************************************************************
    class: AudioSource (audio_source.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "transform.h"
#include "GameplayLayer/gameobject.h"
#include "Core/Audio/audio_manager.h"

namespace Components {

    #define TICK_SKIP_COUNT 4

    //----------------------------------------------------------------------
    AudioSource::AudioSource( AudioClipPtr audioClip, bool playAtStart )
        : m_audioClip( audioClip )
    {
        if (playAtStart)
            m_audioClip->play();
    }

    //----------------------------------------------------------------------
    void AudioSource::AddedToGameObject( GameObject* go )
    {
        _Update3DSettings();
    }

    //----------------------------------------------------------------------
    void AudioSource::Tick( Time::Seconds delta )
    {
        m_tickCount = (m_tickCount + 1) % TICK_SKIP_COUNT;
        if (m_tickCount == 0)
            _Update3DSettings();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void AudioSource::_Update3DSettings()
    {
        auto transform = getComponent<Components::Transform>();

        X3DAUDIO_EMITTER emitter = {};
        emitter.ChannelCount        = INPUTCHANNELS;
        emitter.CurveDistanceScaler = m_innerRadius;
        emitter.OrientFront         = transform->rotation.getForward();
        emitter.OrientTop           = transform->rotation.getUp();
        emitter.Position            = transform->position;

        // Update 3d-settings from audio-clip
        m_audioClip->update3D( emitter );
    }

}