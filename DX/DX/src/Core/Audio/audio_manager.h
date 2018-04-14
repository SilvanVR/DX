#pragma once
/**********************************************************************
    class: AudioManager (audio_manager.h)

    author: S. Hau
    date: April 14, 2018

    Manages audio assets and initialization of the audio lib.
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"

namespace Core { namespace Audio {

    #define INPUTCHANNELS 1  // number of source channels
    #define OUTPUTCHANNELS 8 // maximum number of destination channels supported

    //**********************************************************************
    class AudioManager : public Core::ISubSystem
    {
    public:
        AudioManager() = default;
        ~AudioManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        void update3DVoice(const X3DAUDIO_EMITTER& emitter, IXAudio2SourceVoice* voice, UINT32 sourceChannels);
        void updateListener(const X3DAUDIO_LISTENER& listener);
        void setVolume(F32 volume);

        IXAudio2* getXAudio2() { return m_pXAudio2; }

    private:
        IXAudio2*               m_pXAudio2      = nullptr;
        IXAudio2MasteringVoice* m_pMasterVoice  = nullptr;
        XAUDIO2_VOICE_DETAILS   m_voiceDetails;
        X3DAUDIO_HANDLE         m_X3DInstance;
        F32                     m_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
        X3DAUDIO_LISTENER       m_listener = {};
        X3DAUDIO_DSP_SETTINGS   m_DSPSettings = {};

        //----------------------------------------------------------------------
        AudioManager(const AudioManager& other)               = delete;
        AudioManager& operator = (const AudioManager& other)  = delete;
        AudioManager(AudioManager&& other)                    = delete;
        AudioManager& operator = (AudioManager&& other)       = delete;
    };

} } // End namespaces