#include "wav_clip.h"
/**********************************************************************
    class: WAVClip (wav_clip.cpp)

    author: S. Hau
    date: April 14, 2018
**********************************************************************/

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
    bool WAVClip::load( const OS::Path& path )
    {
        auto extension = path.getExtension();
        if ( extension != "wav" )
        {
            WARN_AUDIO( "WAVClip::load(): Wrong extension '" + extension + "'." );
            return false;
        }

        m_filePath = path;

        HANDLE hFile = CreateFile(
            path.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if ( INVALID_HANDLE_VALUE == hFile )
            return false;

        if ( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
            return false;

        DWORD dwChunkSize;
        DWORD dwChunkPosition;

        // Check the file type, should be fourccWAVE or 'XWMA'
        FindChunk( hFile, fourccRIFF, dwChunkSize, dwChunkPosition );
        DWORD filetype;
        ReadChunkData( hFile, &filetype, sizeof( DWORD ), dwChunkPosition );
        if (filetype != fourccWAVE)
            return false;

        FindChunk( hFile, fourccFMT, dwChunkSize, dwChunkPosition );
        ReadChunkData( hFile, &m_wfx, dwChunkSize, dwChunkPosition );

        // Fill out the audio data buffer with the contents of the fourccDATA chunk
        FindChunk( hFile, fourccDATA, dwChunkSize, dwChunkPosition );
        m_pData.resize( dwChunkSize );
        ReadChunkData( hFile, m_pData.data(), dwChunkSize, dwChunkPosition );

        // Calculate length im seconds
        m_length = (F32)dwChunkSize / m_wfx.Format.nAvgBytesPerSec;

        return true;
    };

} } // End namespaces