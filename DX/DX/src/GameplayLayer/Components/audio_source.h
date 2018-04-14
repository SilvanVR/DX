#pragma once
/**********************************************************************
    class: AudioSource (audio_source.h)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "i_component.h"
#include "Core/Audio/audio_clip.h"

namespace Components {

    //**********************************************************************
    class AudioSource : public Components::IComponent
    {
    public:
        AudioSource(AudioClipPtr audioClip = nullptr, bool playAtStart = true);

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void Tick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        void setClip(AudioClipPtr audioClip){ m_audioClip = audioClip; }
        const AudioClipPtr& getAudioClip() const { return m_audioClip; }

    private:
        AudioClipPtr        m_audioClip;
        X3DAUDIO_EMITTER    m_emitter = {};

        //----------------------------------------------------------------------
        AudioSource(const AudioSource& other)               = delete;
        AudioSource& operator = (const AudioSource& other)  = delete;
        AudioSource(AudioSource&& other)                    = delete;
        AudioSource& operator = (AudioSource&& other)       = delete;
    };

}