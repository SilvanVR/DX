#pragma once
/**********************************************************************
    class: WAVClip (wav_clip.h)

    author: S. Hau
    date: April 14, 2018

    Represents a ".wav" audio clip loaded from disk.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "Time/durations.h"

namespace Core { namespace Audio {

    //**********************************************************************
    class WAVClip
    {
    public:
        WAVClip() = default;
        ~WAVClip() = default;

        //----------------------------------------------------------------------
        // @Return:
        //  True, when file was loaded successfully.
        //----------------------------------------------------------------------
        bool load(const OS::Path& path);

        //----------------------------------------------------------------------
        const WAVEFORMATEXTENSIBLE& getDescription()    const   { return m_wfx; }
        const ArrayList<Byte>&      getData()           const   { return m_pData; }
        const OS::Path&             getFilePath()       const   { return m_filePath; }
        const Time::Seconds         getLength()         const   { return m_length; }

    private:
        OS::Path                m_filePath;
        ArrayList<Byte>         m_pData;
        WAVEFORMATEXTENSIBLE    m_wfx;
        Time::Seconds           m_length;
    };

    using WAVClipPtr = std::shared_ptr<WAVClip>;

} } // End namespaces

