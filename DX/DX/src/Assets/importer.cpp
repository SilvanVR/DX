#include "importer.h"
/**********************************************************************
    class: Importer (importer.cpp)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "Ext/StbImage/stb_image.h"
#include "locator.h"

namespace Assets
{

    //----------------------------------------------------------------------
    Graphics::Texture* Importer::LoadTexture( const OS::Path& filePath )
    {
        //stbi_set_flip_vertically_on_load(1);
        I32 width, height, bpp;
        auto pixels = stbi_load( filePath.c_str(), &width, &height, &bpp, 4 );

        if (pixels)
        {
            auto tex = RESOURCES.createTexture( width, height, Graphics::TextureFormat::RGBA32, true );
            tex->setPixels( pixels );
            tex->apply();

            stbi_image_free( pixels );

            return tex;
        }

        return nullptr;
    }

}