#pragma once
/**********************************************************************
    class: Importer (importer.h)

    author: S. Hau
    date: March 24, 2018

    Class responsible for loading assets from disk.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "Graphics/i_texture2d.hpp"

namespace Assets
{

    //**********************************************************************
    class Importer {
    public:

        //----------------------------------------------------------------------
        // Loads a texture from a file and immediately uploads it to the gpu.
        //----------------------------------------------------------------------
        static Graphics::Texture2D* LoadTexture( const OS::Path& filePath );

    private:
        //----------------------------------------------------------------------
        Importer(const Importer& other)                 = delete;
        Importer& operator = (const Importer& other)    = delete;
        Importer(Importer&& other)                      = delete;
        Importer& operator = (Importer&& other)         = delete;
    };

}