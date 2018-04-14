#pragma once
/**********************************************************************
    class: AudioClip (audio_clip.h)

    author: S. Hau
    date: April 14, 2018

    Manages audio assets and initialization of the audio lib.
**********************************************************************/

#include "OS/FileSystem/path.h"

namespace Core { namespace Audio {

    //**********************************************************************
    class AudioClip
    {
    public:
        AudioClip(const OS::Path& path);
        ~AudioClip();

        void play();

        void pause();

        void setVolume(F32 volume);

        void update3D(const X3DAUDIO_EMITTER& emitter);

        void setBasePitch(F32 basePitch);

    private:
        OS::Path                m_filePath;
        ArrayList<Byte>         m_pData;
        IXAudio2SourceVoice*    m_pSourceVoice = nullptr;
        XAUDIO2_VOICE_DETAILS   m_details;
        WAVEFORMATEXTENSIBLE    m_wfx;
        F32                     m_basePitch = 1.0f;

        //----------------------------------------------------------------------
        AudioClip(const AudioClip& other)               = delete;
        AudioClip& operator = (const AudioClip& other)  = delete;
        AudioClip(AudioClip&& other)                    = delete;
        AudioClip& operator = (AudioClip&& other)       = delete;
    };


} } // End namespaces

using AudioClipPtr = std::shared_ptr<Core::Audio::AudioClip>;