#include "audio_clip.h"
/**********************************************************************
    class: AudioClip (audio_clip.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

#include "locator.h"

namespace Core { namespace Audio {
    
    #define fourccRIFF 'FFIR'
    #define fourccDATA 'atad'
    #define fourccFMT  ' tmf'
    #define fourccWAVE 'EVAW'
    #define fourccXWMA 'AMWX'
    #define fourccDPDS 'sdpd'

    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;

        while (hr == S_OK)
        {
            DWORD dwRead;
            if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;

            default:
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
            }

            dwOffset += sizeof(DWORD) * 2;

            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return S_OK;
            }

            dwOffset += dwChunkDataSize;

            if (bytesRead >= dwRIFFDataSize) return S_FALSE;

        }

        return S_OK;

    }
    HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwRead;
        if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    //----------------------------------------------------------------------
    AudioClip::AudioClip(const OS::Path& path)
        : m_filePath(path) 
    {
        {
            HANDLE hFile = CreateFile(
                path.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

            if (INVALID_HANDLE_VALUE == hFile)
                __debugbreak();

            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
                __debugbreak();

            DWORD dwChunkSize;
            DWORD dwChunkPosition;
            //check the file type, should be fourccWAVE or 'XWMA'
            FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
            DWORD filetype;
            ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
            if (filetype != fourccWAVE)
                __debugbreak();//return S_FALSE;

            FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
            ReadChunkData(hFile, &m_wfx, dwChunkSize, dwChunkPosition);

            //fill out the audio data buffer with the contents of the fourccDATA chunk
            FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
            m_pData.resize(dwChunkSize);
            ReadChunkData(hFile, m_pData.data(), dwChunkSize, dwChunkPosition);
        }

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = (UINT32) m_pData.size();    // size of the audio buffer in bytes 
        buffer.pAudioData = m_pData.data();    // buffer containing audio data
        buffer.Flags = XAUDIO2_END_OF_STREAM;   // tell the source voice not to expect any data after this buffer
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

        if (FAILED( Locator::getAudioManager().getXAudio2()->CreateSourceVoice( &m_pSourceVoice, (WAVEFORMATEX*)&m_wfx) ))
            __debugbreak();

        if (FAILED(m_pSourceVoice->SubmitSourceBuffer(&buffer)))
            __debugbreak();

        m_pSourceVoice->GetVoiceDetails( &m_details );
    };
    //----------------------------------------------------------------------
    AudioClip::~AudioClip() 
    { 
        m_pSourceVoice->DestroyVoice(); 
    }

    //----------------------------------------------------------------------
    void AudioClip::play()
    {
        m_pSourceVoice->FlushSourceBuffers();

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes   = (UINT32) m_pData.size();           // size of the audio buffer in bytes 
        buffer.pAudioData   = m_pData.data();           // buffer containing audio data
        buffer.Flags        = XAUDIO2_END_OF_STREAM;    // tell the source voice not to expect any data after this buffer
        buffer.LoopCount    = XAUDIO2_LOOP_INFINITE;
        m_pSourceVoice->SubmitSourceBuffer(&buffer);

        if (FAILED(m_pSourceVoice->Start()))
            __debugbreak();
    }

    //----------------------------------------------------------------------
    void AudioClip::pause()
    {
        if (FAILED(m_pSourceVoice->Stop()))
            __debugbreak();
    }

    //----------------------------------------------------------------------
    void AudioClip::setVolume(F32 volume)
    {
        if (FAILED(m_pSourceVoice->SetVolume(volume)))
            __debugbreak();
    }

    //----------------------------------------------------------------------
    void AudioClip::update3D(const X3DAUDIO_EMITTER& emitter)
    {
        Locator::getAudioManager().update3DVoice(emitter, m_pSourceVoice, m_details.InputChannels);
    }

    //----------------------------------------------------------------------
    void AudioClip::setBasePitch(F32 basePitch)
    {
        m_basePitch = basePitch;
        m_pSourceVoice->SetFrequencyRatio( m_basePitch );
    }


} } // End namespaces