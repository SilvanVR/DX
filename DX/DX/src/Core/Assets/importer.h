#pragma once
/**********************************************************************
    class: Importer (importer.h)

    author: S. Hau
    date: March 24, 2018

    Class responsible for loading assets from disk.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "Graphics/i_texture2d.hpp"
#include "Graphics/i_cubemap.hpp"
#include "locator.h"

namespace Assets
{

    //**********************************************************************
    class Importer {
    public:

        //----------------------------------------------------------------------
        // Loads a texture from a file.
        //----------------------------------------------------------------------
        static Texture2DPtr LoadTexture(const OS::Path& filePath, bool generateMips = true);

        //----------------------------------------------------------------------
        // Loads several textures to create a cubemap.
        //----------------------------------------------------------------------
        static CubemapPtr LoadCubemap(const OS::Path& posX, const OS::Path& negX,
                                      const OS::Path& posY, const OS::Path& negY, 
                                      const OS::Path& posZ, const OS::Path& negZ, bool generateMips = false);

    private:
        //----------------------------------------------------------------------
        Importer(const Importer& other)                 = delete;
        Importer& operator = (const Importer& other)    = delete;
        Importer(Importer&& other)                      = delete;
        Importer& operator = (Importer&& other)         = delete;
    };

}