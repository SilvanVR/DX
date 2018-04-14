#pragma once
/**********************************************************************
    class: AudioListener (audio_listener.h)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "i_component.h"

namespace Components {

    //**********************************************************************
    class AudioListener : public Components::IComponent
    {
    public:
        AudioListener() = default;

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void AddedToGameObject(GameObject* go) override;
        void Tick(Time::Seconds delta) override;

    private:
        inline void _UpdateListener();

        //----------------------------------------------------------------------
        AudioListener(const AudioListener& other)               = delete;
        AudioListener& operator = (const AudioListener& other)  = delete;
        AudioListener(AudioListener&& other)                    = delete;
        AudioListener& operator = (AudioListener&& other)       = delete;
    };

}