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
        void addedToGameObject(GameObject* go) override;
        void tick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        void                setClip(AudioClipPtr audioClip){ m_audioClip = audioClip; }
        const AudioClipPtr& getAudioClip() const { return m_audioClip; }

        //----------------------------------------------------------------------
        // The area in which this audio source is heard at maximum volume.
        // @Params:
        // "innerRadius": The new inner radius. (Note: Setting this to 0.0f makes it unaudible)
        //----------------------------------------------------------------------
        void setInnerRadius(F32 innerRadius) { m_innerRadius = innerRadius; }

    private:
        AudioClipPtr    m_audioClip;
        F32             m_innerRadius   = 1.0f;
        I32             m_tickCount     = 0; // Used to skip ticks occasionally

        void _Update3DSettings();

        NULL_COPY_AND_ASSIGN(AudioSource)
    };

}