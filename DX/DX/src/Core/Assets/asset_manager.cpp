#include "asset_manager.h"
/**********************************************************************
    class: AssetManager (asset_manager.cpp)

    author: S. Hau
    date: April 9, 2018
**********************************************************************/

#include "Ext/StbImage/stb_image.h"
#include "locator.h"
#include "Common/string_utils.h"
#include "OS/FileSystem/file.h"
#include "Common/string_utils.h"
#include <sstream>

namespace Core { namespace Assets {

    #define HOT_RELOAD_INTERVAL_MILLIS  500
    #define LOG_COLOR                   Color::GREEN
    
    //----------------------------------------------------------------------
    void AssetManager::init()
    {
    }

    //----------------------------------------------------------------------
    void AssetManager::shutdown()
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    Texture2DPtr AssetManager::getTexture2D( const OS::Path& filePath, bool generateMips )
    {
        // Check if texture was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_textureCache.find( pathAsID ) != m_textureCache.end() )
        {
            auto weakPtr = m_textureCache[pathAsID].texture;
            if ( not weakPtr.expired() )
                return Texture2DPtr( weakPtr );
        }

        // Try loading texture
        auto texture = _LoadTexture2D( filePath, generateMips );
        if ( not texture )
        {
            LOG_WARN( "LoadTexture(): Texture '" + filePath.toString() + "' could not be loaded. Returning the default texture instead." );
            return RESOURCES.getWhiteTexture();
        }

        TextureAssetInfo texInfo;
        texInfo.texture     = texture;
        texInfo.path        = filePath;
        texInfo.timeAtLoad  = filePath.getLastWrittenFileTime();

        m_textureCache[pathAsID] = texInfo;

        return texture;
    }

    //----------------------------------------------------------------------
    void AssetManager::getTexture2DAsync( const OS::Path& filePath, bool genMips, const std::function<void(Texture2DPtr)>& callback )
    {
        ASYNC_JOB([=] {
            auto tex = getTexture2D( filePath, genMips );
            callback( tex );
        });
    }

    //----------------------------------------------------------------------
    CubemapPtr AssetManager::getCubemap( const OS::Path& posX, const OS::Path& negX,
                                         const OS::Path& posY, const OS::Path& negY,
                                         const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        // Check if cubemap was already loaded (checks only first path)
        StringID pathAsID = SID( StringUtils::toLower( posX.toString() ).c_str() );
        if ( m_cubemapCache.find( pathAsID ) != m_cubemapCache.end() )
        {
            auto weakPtr = m_cubemapCache[pathAsID].cubemap;
            if ( not weakPtr.expired() )
                return CubemapPtr( weakPtr );
        }

        // Try loading cubemap
        auto cubemap = _LoadCubemap( posX, negX, posY, negY, posZ, negZ, generateMips );
        if (not cubemap)
        {
            LOG_WARN( "LoadCubemap(): At least one of the specified cubemap faces couldn't be loaded! Positive X-Face path was " +
                   posX.toString() + " .Returning default cubemap instead." );
            return RESOURCES.getDefaultCubemap();
        }

        CubemapAssetInfo texInfo;
        texInfo.cubemap     = cubemap;
        texInfo.path        = posX;
        texInfo.timeAtLoad  = posX.getLastWrittenFileTime();

        m_cubemapCache[pathAsID] = texInfo;

        return cubemap;
    }

    //----------------------------------------------------------------------
    AudioClipPtr AssetManager::getAudioClip( const OS::Path& filePath )
    {
        // Check if audio was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_audioCache.find( pathAsID ) != m_audioCache.end() )
        {
            auto weakPtr = m_audioCache[pathAsID].wavClip;
            if ( not weakPtr.expired() )
            {
                auto audioClip = RESOURCES.createAudioClip();
                audioClip->setWAVClip( Audio::WAVClipPtr( weakPtr ) );
                return audioClip;
            }
        }

        // Try loading audio
        LOG( "AssetManager: Loading Audio '" + filePath.toString() + "'", LOG_COLOR );

        auto wav = std::make_shared<Audio::WAVClip>();
        if( not wav->load( filePath ) )
        {
            LOG_WARN( "AssetManager::getAudioClip(): Audio clip '" + filePath.toString() + "' could not be loaded. Returning nullptr." );
            return nullptr;
        }

        auto audioClip = RESOURCES.createAudioClip();
        audioClip->setWAVClip( wav );

        // Cache loaded audio
        AudioClipAssetInfo info;
        info.wavClip    = wav;
        info.path       = filePath;
        info.timeAtLoad = filePath.getLastWrittenFileTime();

        m_audioCache[pathAsID] = info;

        return audioClip;
    }

    //----------------------------------------------------------------------
    ShaderPtr AssetManager::getShader( const OS::Path& filePath )
    {
        // Check if shader was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_shaderCache.find( pathAsID ) != m_shaderCache.end() )
        {
            auto weakPtr = m_shaderCache[pathAsID].shader;
            if ( not weakPtr.expired() )
                return ShaderPtr( weakPtr );
        }

        // Try loading shader
        auto shader = _LoadShader( filePath );
        if ( not shader )
        {
            LOG_WARN( "LoadShader(): Shader '" + filePath.toString() + "' could not be loaded. Returning the error shader instead." );
            return RESOURCES.getErrorShader();
        }

        ShaderAssetInfo shaderInfo;
        shaderInfo.shader      = shader;
        shaderInfo.path        = filePath;
        shaderInfo.timeAtLoad  = filePath.getLastWrittenFileTime();

        m_shaderCache[pathAsID] = shaderInfo;

        return shader;
    }

    //----------------------------------------------------------------------
    void AssetManager::setHotReloading( bool enabled ) 
    { 
        m_hotReloading = enabled;
        if (m_hotReloading)
            _EnableHotReloading();
        else
            Locator::getEngineClock().clearCallback( m_hotReloadingCallback );
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
        LOG( "AssetManager: Loading 6 Cubemap Faces '" + posX.toString() + "' (Positive X-Face) etc.", LOG_COLOR );

        I32 width, height, bpp;
        auto posXPixels = stbi_load( posX.c_str(), &width, &height, &bpp, 4 );
        auto negXPixels = stbi_load( negX.c_str(), &width, &height, &bpp, 4 );
        auto posYPixels = stbi_load( posY.c_str(), &width, &height, &bpp, 4 );
        auto negYPixels = stbi_load( negY.c_str(), &width, &height, &bpp, 4 );
        auto posZPixels = stbi_load( posZ.c_str(), &width, &height, &bpp, 4 );
        auto negZPixels = stbi_load( negZ.c_str(), &width, &height, &bpp, 4 );

        if ( not posXPixels || not negXPixels || not posYPixels || not negYPixels || not posZPixels || not negZPixels )
        {
            stbi_image_free( posXPixels );
            stbi_image_free( negXPixels );
            stbi_image_free( posYPixels );
            stbi_image_free( negYPixels );
            stbi_image_free( posZPixels );
            stbi_image_free( negZPixels );
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

    ShaderPtr AssetManager::_LoadShader( const OS::Path& filePath )
    {
        LOG( "AssetManager: Loading Shader '" + filePath.toString() + "'", LOG_COLOR );

        OS::File file( filePath, OS::EFileMode::READ );
        if ( not file.exists() )
            return nullptr;

        enum class ShaderType
        {
            NONE = 0, VERTEX = 1, FRAGMENT = 2, GEOMETRY = 3, TESSELLATION = 4, NUM_SHADER_TYPES = 5
        } type = ShaderType::NONE;

        // Split file line by line into different strings and handle includes
        std::array<String, (I32)ShaderType::NUM_SHADER_TYPES> shaderSources;
        while ( not file.eof() )
        {
            String line = file.readLine();
            if ( line.find( "#shader" ) != String::npos )
            {
                if ( line.find( "vertex" ) != String::npos )
                    type = ShaderType::VERTEX;
                else if ( line.find( "fragment" ) != String::npos )
                    type = ShaderType::FRAGMENT;
                else if ( line.find( "geometry" ) != String::npos )
                    type = ShaderType::GEOMETRY;
                else if ( line.find( "tessellation" ) != String::npos )
                    type = ShaderType::TESSELLATION;
            }
            else if ( auto pos = line.find( "#include" ) != String::npos )
            {
                auto includeFilePath = StringUtils::substringBetween( line, '\"', '\"' );
                OS::File includeFile( filePath.getDirectoryPath() + includeFilePath );
                if ( not includeFile.exists() )
                    LOG_WARN( "Could not include file '" + includeFilePath + "' in shader-file '" + filePath.toString() + "'." );
                else
                    shaderSources[(I32)type].append( includeFile.readAll() );
            }
            else
            {
                shaderSources[(I32)type].append( line + '\n' );
            }
        }

        auto shader = RESOURCES.createShader();
        shader->setName(filePath.getFileName());

        // Compile each shader
        bool success = false;
        for (I32 i = 1; i < shaderSources.size(); i++)
        {
            if ( not shaderSources[i].empty() )
            {
                switch ((ShaderType)i)
                {
                case ShaderType::VERTEX: 
                    if ( not shader->compileVertexShaderFromSource( shaderSources[i], "main" ) )
                        return nullptr;
                    break;
                case ShaderType::FRAGMENT: 
                    if ( not shader->compileFragmentShaderFromSource( shaderSources[i], "main" ) )
                        return nullptr;
                    break;
                case ShaderType::GEOMETRY:
                case ShaderType::TESSELLATION:
                    ASSERT("Shadertype not supported yet!");
                }
            }
        }

        // Parse & set pipeline states
        StringUtils::IStringStream ss(shaderSources[0]);
        while ( not ss.eof() )
        {
            String line = StringUtils::toLower( ss.nextLine() );
            StringUtils::IStringStream ssLine(line);

            if (line.find("cull") != String::npos)
            {
                if (line.find("front") != String::npos)
                {
                    //shader->setDepthStencilState();
                }
            }

            String word;
            while (ssLine >> word)
            {
                //LOG(word);
            }
        }

        return shader;
    }

    //----------------------------------------------------------------------
    void AssetManager::_EnableHotReloading()
    {
        // HOT-RELOADING CALLBACK
        m_hotReloadingCallback = Locator::getEngineClock().setInterval([this]{

            // Texture reloading
            for ( auto it = m_textureCache.begin(); it != m_textureCache.end(); )
            {
                if ( it->second.texture.expired() )
                {
                    // Texture does no longer exist, so remove it from the cache map
                    it = m_textureCache.erase( it );
                }
                else
                {
                    it->second.ReloadAsyncIfNotUpToDate();
                    it++;
                }
            }

            // Shader reloading
            for (auto it = m_shaderCache.begin(); it != m_shaderCache.end(); )
            {
                if ( it->second.shader.expired() )
                {
                    // Shader does no longer exist, so remove it from the cache map
                    it = m_shaderCache.erase( it );
                }
                else
                {
                    it->second.ReloadAsyncIfNotUpToDate();
                    it++;
                }
            }

        }, HOT_RELOAD_INTERVAL_MILLIS);
    }

    //**********************************************************************
    // PRIVATE - ASSET INFOS
    //**********************************************************************

    //----------------------------------------------------------------------
    void AssetManager::TextureAssetInfo::ReloadAsyncIfNotUpToDate()
    {
        if ( auto tex = texture.lock() )
        {
            try {
                auto currentFileTime = path.getLastWrittenFileTime();

                if (timeAtLoad != currentFileTime)
                {
                    // Reload texture on a separate thread
                    LOG( "Reloading texture: " + path.toString(), LOG_COLOR );
                    ASYNC_JOB([=] {
                        I32 width, height, bpp;
                        auto pixels = stbi_load( path.c_str(), &width, &height, &bpp, 4 );
                        tex->setPixels( pixels );
                        tex->apply();
                        stbi_image_free( pixels );
                    });

                    timeAtLoad = currentFileTime;
                }
            }
            catch (...) {
                // Do nothing here. This means simply the file could not be opened, because another app has not yet closed the handle
            }
        }
    }

    //----------------------------------------------------------------------
    void AssetManager::ShaderAssetInfo::ReloadAsyncIfNotUpToDate()
    {
        if ( auto sh = shader.lock() )
        {
            try {
                auto currentFileTime = path.getLastWrittenFileTime();

                if (timeAtLoad != currentFileTime)
                {
                    // Reload shader on a separate thread
                    LOG( "Reloading shader: " + path.toString(), LOG_COLOR );
                    ASYNC_JOB([=] {
                        // @TODO
                    });

                    timeAtLoad = currentFileTime;
                }
            }
            catch (...) {
                // Do nothing here. This means simply the file could not be opened, because another app has not yet closed the handle
            }
        }
    }

} } // End namespaces