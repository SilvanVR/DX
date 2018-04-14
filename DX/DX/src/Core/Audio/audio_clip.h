#pragma once
/**********************************************************************
    class: AudioClip (audio_clip.h)

    author: S. Hau
    date: April 14, 2018

    Manages audio assets and initialization of the audio lib.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "wav_clip.h"

namespace Core { namespace Audio {

    //**********************************************************************
    class AudioClip
    {
    public:
        AudioClip() = default;
        ~AudioClip();

        //----------------------------------------------------------------------
        bool isLooping() const { return m_isLooping; }
        F32 getVolume() const { F32 volume; m_pSourceVoice->GetVolume(&volume); return volume; }

        //----------------------------------------------------------------------
        // Starts playing this clip from the beginning.
        //----------------------------------------------------------------------
        void play();

        //----------------------------------------------------------------------
        // Stops the clip. Next time clip starts from the beginning.
        //----------------------------------------------------------------------
        void stop();

        //----------------------------------------------------------------------
        // Pauses the clip. Can be resumed with resume().
        //----------------------------------------------------------------------
        void pause();

        //----------------------------------------------------------------------
        // Resumes this clip when it was previously paused.
        //----------------------------------------------------------------------
        void resume();

        //----------------------------------------------------------------------
        // Adjust the volume for this clip.
        //----------------------------------------------------------------------
        void setVolume(F32 volume);

        //----------------------------------------------------------------------
        // Change the base pitch for this clip.
        //----------------------------------------------------------------------
        void setBasePitch(F32 basePitch);

        //----------------------------------------------------------------------
        // Change the name for this audio clip. Automatically set to the filepath.
        //----------------------------------------------------------------------
        void setName(const String& name) { m_name = name; }

        //----------------------------------------------------------------------
        // Set whether this audio clip will loop or not.
        //----------------------------------------------------------------------
        void setIsLooping(bool isLooping);


        void update3D(const X3DAUDIO_EMITTER& emitter);
        void setWAVClip(const WAVClip& wavClip);

    private:
        String                  m_name;
        IXAudio2SourceVoice*    m_pSourceVoice = nullptr;
        XAUDIO2_VOICE_DETAILS   m_details;
        F32                     m_basePitch = 1.0f;
        bool                    m_isLooping = false;
        WAVClip                 m_wavClip;

        inline void _SubmitSourceBuffer();

        //----------------------------------------------------------------------
        AudioClip(const AudioClip& other)               = delete;
        AudioClip& operator = (const AudioClip& other)  = delete;
        AudioClip(AudioClip&& other)                    = delete;
        AudioClip& operator = (AudioClip&& other)       = delete;
    };


} } // End namespaces

using AudioClipPtr = std::shared_ptr<Core::Audio::AudioClip>;