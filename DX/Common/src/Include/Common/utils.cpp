#include "utils.h"

#include <sstream>

namespace Utils
{

    //----------------------------------------------------------------------
    String bytesToString( U64 bytes, bool binaryPrefix )
    {
        static const U64 gigabyte = 1000 * 1000 * 1000;
        static const U64 megabyte = 1000 * 1000;
        static const U64 kilobyte = 1000;

        static const U64 gibibyte = 1024 * 1024 * 1024;
        static const U64 mebibyte = 1024 * 1024;
        static const U64 kibibyte = 1024;

        U64 gb = binaryPrefix ? gibibyte : gigabyte;
        U64 mb = binaryPrefix ? mebibyte : megabyte;
        U64 kb = binaryPrefix ? kibibyte : kilobyte;

        String result;
        if (bytes > gb)
            result = TS((float)bytes / gb) + (binaryPrefix ? " GiB" : " GB");
        else if (bytes > mb)
            result = TS((float)bytes / mb) + (binaryPrefix ? " MiB" : " MB");
        else if (bytes > kb)
            result = TS((float)bytes / kb) + (binaryPrefix ? " KiB" : " KB");
        else
            result = TS((float)bytes) + " Bytes";

        return result;
    }

    //----------------------------------------------------------------------
    String memoryAddressToString( void* address )
    {
        std::ostringstream stream;
        stream << address;

        return "0x" + stream.str();
    }



}