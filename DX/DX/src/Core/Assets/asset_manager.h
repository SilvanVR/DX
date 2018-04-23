#pragma once
/**********************************************************************
    class: AssetManager (asset_manager.h)

    author: S. Hau
    date: April 9, 2018

    Manages the loading of assets from disk:
    - Ensures that every asset is loaded only once.
    - Async loading functions if desired.
    - Resource reloading if enabled.
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "OS/FileSystem/path.h"
#include "Graphics/i_texture2d.hpp"
#include "Graphics/i_cubemap.hpp"
#include "Core/Audio/audio_clip.h"

namespace Core { namespace Assets {

    //**********************************************************************
    class AssetManager : public ISubSystem
    {
        using WeakTexture2DPtr  = std::weak_ptr<Graphics::Texture2D>;
        using WeakCubemapPtr    = std::weak_ptr<Graphics::Cubemap>;
        using WeakWavClipPtr    = std::weak_ptr<Audio::WAVClip>;

    public:
        AssetManager() = default;
        ~AssetManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Creates a new 2d texture from a file. Will be loaded only if not already in memory.
        // Returns a default one and issues a warning if the file couldn't be loaded.
        // @Params:
        //  "path": Path to the texture.
        //  "genMips": If true a complete mipchain will be generated.
        //----------------------------------------------------------------------
        Texture2DPtr getTexture2D(const OS::Path& filePath, bool genMips = true);
        void getTexture2DAsync(const OS::Path& filePath, bool genMips, const std::function<void(Texture2DPtr)>& callback);

        //----------------------------------------------------------------------
        // Creates a new cubemap from a file. Will be loaded only if not already in memory. (Checks only first path)
        // Returns a default one and issues a warning if file couldn't be loaded.
        // @Params:
        //  "pos?": Path to each cubemap face.
        //  "genMips": If true a complete mipchain will be generated for each face.
        //----------------------------------------------------------------------
        CubemapPtr getCubemap(const OS::Path& posX, const OS::Path& negX,
                              const OS::Path& posY, const OS::Path& negY, 
                              const OS::Path& posZ, const OS::Path& negZ, bool genMips = false);

        //----------------------------------------------------------------------
        // Creates a new audio object. Will be loaded only if not already in memory.
        // @Params:
        //  "path": Path to the audio clip.
        //----------------------------------------------------------------------
        AudioClipPtr getAudioClip(const OS::Path& path);

        //----------------------------------------------------------------------
        // Enable/Disable hot reloading. The asset manager will periodically check
        // all loaded resource files and reload them if they are outdated. (Note that not all resource types are supported)
        //----------------------------------------------------------------------
        void setHotReloading(bool enabled);

    private:
        CallbackID m_hotReloadingCallback = 0;
        bool m_hotReloading = false;

        struct FileInfo
        {
            OS::Path            path;
            OS::SystemTime      timeAtLoad;
        };

        struct TextureAssetInfo : public FileInfo
        {
            WeakTexture2DPtr    texture;

            // Reloads the texture on a separate thread if not up to date.
            void ReloadAsyncIfNotUpToDate();
        };

        struct CubemapAssetInfo : public FileInfo
        {
            WeakCubemapPtr cubemap;
            // No reloading supported for cubemaps
        };

        struct AudioClipAssetInfo : public FileInfo
        {
            WeakWavClipPtr wavClip;
            // No reloading supported for audio
        };

        // Lists of all loaded resources. Stores weak-ptrs, which means that the resource might be already unloaded.
        HashMap<StringID, TextureAssetInfo>     m_textureCache;
        HashMap<StringID, CubemapAssetInfo>     m_cubemapCache;
        HashMap<StringID, AudioClipAssetInfo>   m_audioCache;

        //----------------------------------------------------------------------
        inline Texture2DPtr _LoadTexture2D(const OS::Path& filePath, bool generateMips);
        inline CubemapPtr _LoadCubemap(const OS::Path& posX, const OS::Path& negX, 
                                       const OS::Path& posY, const OS::Path& negY,
                                       const OS::Path& posZ, const OS::Path& negZ, bool generateMips);
        void _EnableHotReloading();

        //----------------------------------------------------------------------
        AssetManager(const AssetManager& other)                 = delete;
        AssetManager& operator = (const AssetManager& other)    = delete;
        AssetManager(AssetManager&& other)                      = delete;
        AssetManager& operator = (AssetManager&& other)         = delete;
    };

} } // End namespaces