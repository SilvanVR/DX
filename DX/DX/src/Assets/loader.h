#pragma once
/**********************************************************************
    class: Loader (loader.h)

    author: S. Hau
    date: March 24, 2018

    Class responsible for loading assets from disk.
**********************************************************************/

#include "OS/FileSystem/path.h"

namespace Assets
{

    //**********************************************************************
    class Loader {
    public:

        //static Texture* LoadTexture( const OS::Path& filePath );

    private:

        //----------------------------------------------------------------------
        Loader(const Loader& other)                 = delete;
        Loader& operator = (const Loader& other)    = delete;
        Loader(Loader&& other)                      = delete;
        Loader& operator = (Loader&& other)         = delete;
    };

}