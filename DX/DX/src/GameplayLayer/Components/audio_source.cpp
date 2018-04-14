#include "audio_source.h"
/**********************************************************************
    class: AudioSource (audio_source.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "transform.h"
#include "GameplayLayer/gameobject.h"

namespace Components {

    //----------------------------------------------------------------------
    AudioSource::AudioSource( AudioClipPtr audioClip, bool playAtStart )
        : m_audioClip( audioClip )
    {
        if (playAtStart)
            m_audioClip->play();
    }

    //----------------------------------------------------------------------
    void AudioSource::Tick(Time::Seconds delta)
    {
        auto transform = getComponent<Components::Transform>();

        //m_emitter.ChannelCount        = INPUTCHANNELS;
        m_emitter.ChannelCount        = 1;
        m_emitter.CurveDistanceScaler = 1.0f;
        m_emitter.OrientFront         = transform->rotation.getForward();
        m_emitter.OrientTop           = transform->rotation.getUp();
        m_emitter.Position            = transform->position;
        m_emitter.Velocity            = { 0,0,0 };
        m_emitter.InnerRadius         = 5.0f;
        m_emitter.InnerRadiusAngle    = X3DAUDIO_PI / 4.0;

        // Update 3d-settings from audio-clip
        m_audioClip->update3D( m_emitter );
    }

}