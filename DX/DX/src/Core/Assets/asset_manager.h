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

namespace Core { namespace Assets {

    //**********************************************************************
    class AssetManager : public ISubSystem
    {
        using WeakTexture2DPtr = std::weak_ptr<Graphics::Texture2D>;

    public:
        AssetManager() = default;
        ~AssetManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Creates a new 2d texture from a file. Returns a default one and issues a warning if the file couldn't be loaded.
        // @Params:
        //  "path": Path to the texture.
        //  "genMips": If true a complete mipchain will be generated.
        //----------------------------------------------------------------------
        Texture2DPtr getTexture2D(const OS::Path& filePath, bool generateMips = true);

        //----------------------------------------------------------------------
        // Creates a new cubemap from a file. Returns a default one and issues a warning if file couldn't be loaded.
        // @Params:
        //  "path": Path to the texture.
        //  "genMips": If true a complete mipchain will be generated.
        //----------------------------------------------------------------------
        CubemapPtr getCubemap(const OS::Path& posX, const OS::Path& negX,
                               const OS::Path& posY, const OS::Path& negY, 
                               const OS::Path& posZ, const OS::Path& negZ, bool generateMips = false);

    private:

        // Contains <Path,Ptr> to all textures. They might be already unloaded.
        HashMap<StringID, WeakTexture2DPtr> m_textureCache;

        struct FileInfo
        {
            OS::Path        path;
            OS::SystemTime  timeAtLoad;
        };
        HashMap<Graphics::Texture2D*, FileInfo> m_textureFileInfo;


        //----------------------------------------------------------------------
        inline Texture2DPtr _LoadTexture2D(const OS::Path& filePath, bool generateMips);
        inline CubemapPtr _LoadCubemap(const OS::Path& posX, const OS::Path& negX, 
                                       const OS::Path& posY, const OS::Path& negY,
                                       const OS::Path& posZ, const OS::Path& negZ, bool generateMips);

        //----------------------------------------------------------------------
        AssetManager(const AssetManager& other)                 = delete;
        AssetManager& operator = (const AssetManager& other)    = delete;
        AssetManager(AssetManager&& other)                      = delete;
        AssetManager& operator = (AssetManager&& other)         = delete;
    };

} } // End namespaces