#include "audio_manager.h"
/**********************************************************************
    class: AudioManager (audio_manager.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "Events/event_dispatcher.h"

namespace Core { namespace Audio {

    //----------------------------------------------------------------------
    void AudioManager::init()
    {
        // XAUDIO2
        if ( FAILED( XAudio2Create( &m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
        {
            LOG_WARN_AUDIO( "XAudio2Create(): Initialization failed. Audio won't work." );
            return;
        }

        if ( FAILED( m_pXAudio2->CreateMasteringVoice( &m_pMasterVoice ) ) )
        {
            LOG_WARN_AUDIO( "XAudio2(): Could not create a master voice, which is needed for output." );
            return;
        }

        // X3DAUDIO
        DWORD dwChannelMask;
        m_pMasterVoice->GetChannelMask( &dwChannelMask );

        if ( FAILED( X3DAudioInitialize( dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance ) ) )
            LOG_WARN_AUDIO( "X3DAudioInitialize(): Initialization failed. 3D Audio won't work." );

        m_pMasterVoice->GetVoiceDetails( &m_voiceDetails );

        // DSP settings
        m_DSPSettings.SrcChannelCount       = INPUTCHANNELS;
        m_DSPSettings.DstChannelCount       = m_voiceDetails.InputChannels;
        m_DSPSettings.pMatrixCoefficients   = m_matrixCoefficients;
    }

    //----------------------------------------------------------------------
    void AudioManager::shutdown()
    {
        if (m_pMasterVoice)
            m_pMasterVoice->DestroyVoice();
        if (m_pXAudio2)
            m_pXAudio2->StopEngine();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void AudioManager::update3DVoice( const X3DAUDIO_EMITTER& emitter, IXAudio2SourceVoice* voice, UINT32 sourceChannels )
    {
        X3DAudioCalculate( m_X3DInstance, &m_listener, &emitter, X3DAUDIO_CALCULATE_MATRIX, &m_DSPSettings );

        if ( FAILED( voice->SetOutputMatrix( NULL, sourceChannels, m_voiceDetails.InputChannels, m_DSPSettings.pMatrixCoefficients ) ) )
            LOG_WARN_AUDIO( "AudioManager::update3DVoice(): Failed to set the output matrix for a source voice." );
    }

    //----------------------------------------------------------------------
    void AudioManager::updateListener( const X3DAUDIO_LISTENER& listener )
    {
        m_listener = listener;
    }

    //----------------------------------------------------------------------
    void AudioManager::setVolume( F32 volume )
    {
        if ( FAILED( m_pMasterVoice->SetVolume( volume ) ) )
            LOG_WARN_AUDIO( "AudioManager::setVolume(): Could not set the master volume." );
    }

    //----------------------------------------------------------------------
    F32 AudioManager::getVolume() const
    {
        F32 volume;
        m_pMasterVoice->GetVolume( &volume );
        return volume;
    }

} } // End namespaces