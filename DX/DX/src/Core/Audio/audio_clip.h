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
        bool    isLooping()     const { return m_isLooping; }
        F32     getVolume()     const { F32 volume; m_pSourceVoice->GetVolume(&volume); return volume; }
        F32     getBasePitch()  const { return m_basePitch; }

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
        // Set whether this audio clip will loop or not.
        //----------------------------------------------------------------------
        void setIsLooping(bool isLooping);

        //----------------------------------------------------------------------
        // Update the 3D-Settings for this clip.
        //----------------------------------------------------------------------
        void update3D(const X3DAUDIO_EMITTER& emitter);

        //----------------------------------------------------------------------
        // Set the wavclip for this audio-clip, which contains the actual audio data.
        //----------------------------------------------------------------------
        void setWAVClip(const WAVClip& wavClip);

    private:
        IXAudio2SourceVoice*    m_pSourceVoice = nullptr;
        XAUDIO2_VOICE_DETAILS   m_details;
        F32                     m_basePitch = 1.0f;
        WAVClip                 m_wavClip;
        bool                    m_isLooping = false;

        inline void _SubmitSourceBuffer();

        //----------------------------------------------------------------------
        AudioClip(const AudioClip& other)               = delete;
        AudioClip& operator = (const AudioClip& other)  = delete;
        AudioClip(AudioClip&& other)                    = delete;
        AudioClip& operator = (AudioClip&& other)       = delete;
    };


} } // End namespaces

using AudioClipPtr = std::shared_ptr<Core::Audio::AudioClip>;