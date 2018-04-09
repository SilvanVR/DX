#include "asset_manager.h"
/**********************************************************************
    class: AssetManager (asset_manager.cpp)

    author: S. Hau
    date: April 9, 2018
**********************************************************************/

#include "Ext/StbImage/stb_image.h"
#include "locator.h"

namespace Core { namespace Assets {

    #define LOG_COLOR Color::BLUE
    
    //----------------------------------------------------------------------
    void AssetManager::init()
    {
        // HOT-RELOADING CALLBACK
        Locator::getEngineClock().setInterval([this]{

            // Texture reloading
            for (auto& pair : m_textureFileInfo)
            {
                auto& texture   = pair.first;
                auto& fileInfo  = pair.second;

                auto currentFileTime = fileInfo.path.getLastWrittenFileTime();
                if ( fileInfo.timeAtLoad != currentFileTime )
                {
                    // Reload texture
                    LOG( "Reloading texture: " + fileInfo.path.toString() );
                    //texture->setPixels(nullptr);

                    fileInfo.timeAtLoad = currentFileTime;
                }
            }

        }, 500);
    }

    //----------------------------------------------------------------------
    void AssetManager::shutdown()
    {

    }

    //----------------------------------------------------------------------
    //Texture2DPtr ResourceManager::getTexture2D(const OS::Path& path, bool genMips)
    //{
    //    StringID pathAsID = SID(path.c_str());
    //    if (m_textureCache.find(pathAsID) != m_textureCache.end())
    //    {
    //        auto weakPtr = m_textureCache[pathAsID];

    //        if (not weakPtr.expired())
    //        {
    //            LOG("Texture '" + path.toString() + "' already loaded!");
    //            return Texture2DPtr(weakPtr);
    //        }
    //    }

    //    auto tex = Assets::Importer::LoadTexture(path, genMips);

    //    //@TODO: handle load failure
    //    m_textureCache[pathAsID] = tex;

    //    FileInfo fileInfo;
    //    fileInfo.path = path;
    //    fileInfo.timeAtLoad = path.getLastWrittenFileTime();
    //    m_textureFileInfo[tex.get()] = fileInfo;

    //    return tex;
    //}

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    Texture2DPtr AssetManager::getTexture2D( const OS::Path& filePath, bool generateMips )
    {
        auto texture = _LoadTexture2D( filePath, generateMips );
        if ( not texture )
        {
            WARN( "LoadTexture(): Texture '" + filePath.toString() + "' could not be loaded. Returning the default texture instead." );
            return RESOURCES.getWhiteTexture();
        }

        return texture;
    }

    //----------------------------------------------------------------------
    CubemapPtr AssetManager::getCubemap( const OS::Path& posX, const OS::Path& negX,
                                         const OS::Path& posY, const OS::Path& negY,
                                         const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        auto cubemap = _LoadCubemap( posX, negX, posY, negY, posZ, negZ, generateMips );
        if (not cubemap)
        {
            WARN( "LoadCubemap(): At least one of the specified cubemap faces couldn't be loaded! Positive X-Face path was " +
                   posX.toString() + " .Returning default cubemap instead." );
            return RESOURCES.getDefaultCubemap();
        }

        return cubemap;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    Texture2DPtr AssetManager::_LoadTexture2D( const OS::Path& filePath, bool generateMips )
    {
        LOG( "AssetManager: Loading Texture '" + filePath.toString() + "'", LOG_COLOR );

        I32 width, height, bpp;
        auto pixels = stbi_load( filePath.c_str(), &width, &height, &bpp, 4 );

        if (pixels)
        {
            auto tex = RESOURCES.createTexture2D( width, height, Graphics::TextureFormat::RGBA32, generateMips );
            tex->setPixels( pixels );
            tex->apply();

            stbi_image_free( pixels );

            return tex;
        }

        return nullptr;
    }

    //----------------------------------------------------------------------
    CubemapPtr AssetManager::_LoadCubemap( const OS::Path& posX, const OS::Path& negX,
                                           const OS::Path& posY, const OS::Path& negY,
                                           const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        LOG( "AssetManager: Loading Cubemap '" + posX.toString() + "' (Positive X-Face) etc.", LOG_COLOR );

        I32 width, height, bpp;
        auto posXPixels = stbi_load( posX.c_str(), &width, &height, &bpp, 4 );
        auto negXPixels = stbi_load( negX.c_str(), &width, &height, &bpp, 4 );
        auto posYPixels = stbi_load( posY.c_str(), &width, &height, &bpp, 4 );
        auto negYPixels = stbi_load( negY.c_str(), &width, &height, &bpp, 4 );
        auto posZPixels = stbi_load( posZ.c_str(), &width, &height, &bpp, 4 );
        auto negZPixels = stbi_load( negZ.c_str(), &width, &height, &bpp, 4 );

        if ( not posXPixels || not negXPixels || not posYPixels || not negYPixels || not posZPixels || not negZPixels )
            return nullptr;

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


} } // End namespaces