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
    Graphics::Texture2D* Importer::LoadTexture( const OS::Path& filePath )
    {
        //stbi_set_flip_vertically_on_load(1);
        I32 width, height, bpp;
        auto pixels = stbi_load( filePath.c_str(), &width, &height, &bpp, 4 );

        if (pixels)
        {
            auto tex = RESOURCES.createTexture2D( width, height, Graphics::TextureFormat::RGBA32, true );
            tex->setPixels( pixels );
            tex->apply();

            stbi_image_free( pixels );

            return tex;
        }

        return nullptr;
    }

    //----------------------------------------------------------------------
    Graphics::Cubemap* Importer::LoadCubemap( const OS::Path& posX, const OS::Path& negX,
                                              const OS::Path& posY, const OS::Path& negY,
                                              const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        I32 width, height, bpp;
        auto posXPixels = stbi_load( posX.c_str(), &width, &height, &bpp, 4 );
        auto negXPixels = stbi_load( negX.c_str(), &width, &height, &bpp, 4 );
        auto posYPixels = stbi_load( posY.c_str(), &width, &height, &bpp, 4 );
        auto negYPixels = stbi_load( negY.c_str(), &width, &height, &bpp, 4 );
        auto posZPixels = stbi_load( posZ.c_str(), &width, &height, &bpp, 4 );
        auto negZPixels = stbi_load( negZ.c_str(), &width, &height, &bpp, 4 );

        if ( not posXPixels || not negXPixels || not posYPixels || not negYPixels || not posZPixels || not negZPixels )
        { 
            WARN( "LoadCubemap(): At least one of the specified cubemap faces couln't be loaded!" );
            return nullptr;
        }

        auto cubemap = RESOURCES.createCubemap();
        cubemap->create( width, Graphics::TextureFormat::RGBA32, generateMips );
      
        cubemap->setPixels( Graphics::CubemapFace::PositiveX, posXPixels );
        cubemap->setPixels( Graphics::CubemapFace::NegativeX, negXPixels );
        cubemap->setPixels( Graphics::CubemapFace::PositiveY, posYPixels );
        cubemap->setPixels( Graphics::CubemapFace::NegativeY, negYPixels );
        cubemap->setPixels( Graphics::CubemapFace::PositiveZ, posZPixels );
        cubemap->setPixels( Graphics::CubemapFace::NegativeZ, negZPixels );

        stbi_image_free( posXPixels );
        stbi_image_free( negXPixels );
        stbi_image_free( posYPixels );
        stbi_image_free( negYPixels );
        stbi_image_free( posZPixels );
        stbi_image_free( negZPixels );

        cubemap->apply();
        return cubemap;
    }

}