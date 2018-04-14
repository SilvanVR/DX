#include "audio_clip.h"
/**********************************************************************
    class: AudioClip (audio_clip.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "locator.h"

namespace Core { namespace Audio {

    //----------------------------------------------------------------------
    AudioClip::~AudioClip() 
    { 
        m_pSourceVoice->DestroyVoice(); 
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void AudioClip::setWAVClip(const WAVClip& wavClip)
    {
        m_wavClip   = wavClip;
        m_name      = m_wavClip.getFilePath();

        if ( FAILED( Locator::getAudioManager().getXAudio2()->CreateSourceVoice( &m_pSourceVoice, (WAVEFORMATEX*)&m_wavClip.getDescription() ) ) )
            __debugbreak();

        _SubmitSourceBuffer();
        m_pSourceVoice->GetVoiceDetails( &m_details );
    }

    //----------------------------------------------------------------------
    void AudioClip::play()
    {
        stop();
        resume();
    }

    //----------------------------------------------------------------------
    void AudioClip::stop()
    {
        if ( FAILED( m_pSourceVoice->Stop() ) )
            WARN_AUDIO( "Failed to stop audio clip '" + m_name + "'." );
        m_pSourceVoice->FlushSourceBuffers();
        _SubmitSourceBuffer();
    }

    //----------------------------------------------------------------------
    void AudioClip::pause()
    {
        if ( FAILED( m_pSourceVoice->Stop() ) )
            WARN_AUDIO( "Failed to pause audio clip '" + m_name + "'." );
    }

    //----------------------------------------------------------------------
    void AudioClip::resume()
    {
        if ( FAILED( m_pSourceVoice->Start() ) )
            WARN_AUDIO( "Failed to resume audio clip '" + m_name + "'." );
    }

    //----------------------------------------------------------------------
    void AudioClip::setVolume(F32 volume)
    {
        if ( FAILED( m_pSourceVoice->SetVolume( volume ) ) )
            WARN_AUDIO( "Failed to set volume from audio clip '" + m_name + "'.");
    }

    //----------------------------------------------------------------------
    void AudioClip::update3D(const X3DAUDIO_EMITTER& emitter)
    {
        Locator::getAudioManager().update3DVoice(emitter, m_pSourceVoice, m_details.InputChannels);
    }

    //----------------------------------------------------------------------
    void AudioClip::setBasePitch( F32 basePitch )
    {
        m_basePitch = basePitch;
        m_pSourceVoice->SetFrequencyRatio( m_basePitch );
    }

    //----------------------------------------------------------------------
    void AudioClip::setIsLooping(bool isLooping)
    { 
        m_isLooping = isLooping; 
        if( not m_isLooping )
            m_pSourceVoice->ExitLoop();
        else
            _SubmitSourceBuffer();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void AudioClip::_SubmitSourceBuffer()
    {
        ASSERT( not m_wavClip.getData().empty() );

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes   = (UINT32) m_wavClip.getData().size();
        buffer.pAudioData   = m_wavClip.getData().data();
        buffer.Flags        = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
        buffer.LoopCount    = m_isLooping ? XAUDIO2_LOOP_INFINITE : 0;
        
        if ( FAILED( m_pSourceVoice->SubmitSourceBuffer( &buffer ) ) )
            WARN_AUDIO( "AudioClip: Failed to submit a source buffer. Clip '" + m_name + "' won't play." );
    }

} } // End namespaces