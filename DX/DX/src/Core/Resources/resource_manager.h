#pragma once
/**********************************************************************
    class: ResourceManager (resource_manager.h)

    author: S. Hau
    date: December 25, 2017

    Manages all resources in the game:
    - Creates & Stores them
    - Deletes them

    Ideas:
    Resource-Table in the background with a counter
    -> If counter reaches zero, delete resource (possibly with a delay e.g. after 5sec unused)
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"
#include "Graphics/i_shader.hpp"
#include "Graphics/i_texture2d.hpp"
#include "Graphics/i_render_texture.hpp"
#include "Graphics/i_cubemap.hpp"
#include "Graphics/i_texture2d_array.hpp"

namespace Core { namespace Resources {

    const static CString SHADER_DEFAULT_NAME    = "DEFAULT";
    const static CString SHADER_WIREFRAME_NAME  = "WIREFRAME";
    const static CString SHADER_ERROR_NAME      = "ERROR";

    //*********************************************************************
    class ResourceManager : public ISubSystem
    {
        using WeakTexturePtr    = std::weak_ptr<Graphics::Texture>;
        using WeakTexture2DPtr  = std::weak_ptr<Graphics::Texture2D>;
        using WeakMaterialPtr   = std::weak_ptr<Graphics::Material>;
        using WeakMeshPtr       = std::weak_ptr<Graphics::Mesh>;
        using WeakShaderPtr     = std::weak_ptr<Graphics::Shader>;

    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Creates a new mesh
        //----------------------------------------------------------------------
        MeshPtr createMesh();
        MeshPtr createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices);
        MeshPtr createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices, const ArrayList<Math::Vec2>& uvs);

        //----------------------------------------------------------------------
        // Creates a new material
        // @Params:
        //  "shader": Shader to use for this material. If null default shader will be applied.
        //----------------------------------------------------------------------
        MaterialPtr createMaterial(ShaderPtr shader = nullptr);

        //----------------------------------------------------------------------
        // Creates a new shader
        //----------------------------------------------------------------------
        ShaderPtr createShader(CString name, const OS::Path& vertPath, const OS::Path& fragPath);

        //----------------------------------------------------------------------
        // Creates a new 2d texture
        // @Params:
        //  "width": Width of the texture in pixels
        //  "height": Height of the texture in pixels
        //  "format": The format of the texture
        //  "generateMips": If true a complete mipchain will be generated
        //----------------------------------------------------------------------
        Texture2DPtr createTexture2D(U32 width, U32 height, Graphics::TextureFormat format, bool generateMips = true);

        //----------------------------------------------------------------------
        // Creates a new immutable texture
        // @Params:
        //  "width": Width of the texture in pixels
        //  "height": Height of the texture in pixels
        //  "format": The format of the texture
        //  "pData": Pointer to initial texture data. If this is not null the
        //           texture will be immutable.
        //----------------------------------------------------------------------
        Texture2DPtr createTexture2D(U32 width, U32 height, Graphics::TextureFormat format, const void* pData);

        //----------------------------------------------------------------------
        // Creates a new texture
        // @Params:
        //  "width": Width of the texture in pixels
        //  "height": Height of the texture in pixels
        //  "depth": Number of textures in the array
        //  "format": The format of the texture
        //  "generateMips": If true a complete mipchain will be generated
        //----------------------------------------------------------------------
        Texture2DArrayPtr createTexture2DArray(U32 width, U32 height, U32 depth, Graphics::TextureFormat format, bool generateMips = true);

        //----------------------------------------------------------------------
        // Creates a new render texture
        //----------------------------------------------------------------------
        RenderTexturePtr createRenderTexture();

        //----------------------------------------------------------------------
        // Creates a new cubemap
        //----------------------------------------------------------------------
        CubemapPtr createCubemap();

        //----------------------------------------------------------------------
        // Creates a new 2d texture from a file. Returns a default one and issues a warning if file couldn't be loaded.
        // @Params:
        //  "path": Path to the texture.
        //  "genMips": If true a complete mipchain will be generated.
        //----------------------------------------------------------------------
        Texture2DPtr getTexture2D(const OS::Path& path, bool genMips = true);

        //----------------------------------------------------------------------
        // Creates a new cubemap from a file. Returns a default one and issues a warning if file couldn't be loaded.
        // @Params:
        //  "path": Path to the texture.
        //  "genMips": If true a complete mipchain will be generated.
        //----------------------------------------------------------------------
        CubemapPtr getCubemap(const OS::Path& posX, const OS::Path& negX,
                              const OS::Path& posY, const OS::Path& negY,
                              const OS::Path& posZ, const OS::Path& negZ, bool generateMips = false);

        //----------------------------------------------------------------------
        // Sets the anisotropic filtering for all textures @TODO: move this somewhere else
        //----------------------------------------------------------------------
        void setGlobalAnisotropicFiltering(U32 level);

        //----------------------------------------------------------------------
        // Unloads every resource which is not being used by anybody
        //----------------------------------------------------------------------
        void UnloadUnusedResources();

        //----------------------------------------------------------------------
        U32 getMeshCount()      const { return static_cast<U32>( m_meshes.size() ); }
        U32 getShaderCount()    const { return static_cast<U32>( m_shaders.size() ); }
        U32 getMaterialCount()  const { return static_cast<U32>( m_materials.size() ); }
        U32 getTextureCount()   const { return static_cast<U32>( m_textures.size() ); }

        const MaterialPtr&      getDefaultMaterial()    const { return m_defaultMaterial; }
        const ShaderPtr&        getDefaultShader()      const { return m_defaultShader; }

        const Texture2DPtr&     getBlackTexture() const { return m_black; }
        const Texture2DPtr&     getWhiteTexture() const { return m_white; }


    private:
        ArrayList<Graphics::Mesh*>      m_meshes;
        ArrayList<Graphics::Shader*>    m_shaders;
        ArrayList<Graphics::Material*>  m_materials;
        ArrayList<Graphics::Texture*>   m_textures;

        ShaderPtr       m_defaultShader;
        ShaderPtr       m_errorShader;
        ShaderPtr       m_wireframeShader;

        MaterialPtr     m_defaultMaterial;
        MaterialPtr     m_wireframeMaterial;

        Texture2DPtr    m_black;
        Texture2DPtr    m_white;

        // Contains <Path,Ptr> to all textures. They might be already unloaded.
        HashMap<StringID, WeakTexture2DPtr> m_textureCache;

        struct FileInfo
        {
            OS::Path        path;
            OS::SystemTime  timeAtLoad;
        };

        HashMap<Graphics::Texture2D*, FileInfo> m_textureFileInfo;

        //----------------------------------------------------------------------
        void _CreateDefaultAssets();
        void _OnSceneChanged();

        void _DeleteTexture(Graphics::Texture* tex);
        void _DeleteMesh(Graphics::Mesh* mesh);
        void _DeleteMaterial(Graphics::Material* mat);
        void _DeleteShader(Graphics::Shader* shader);

        //----------------------------------------------------------------------
        ResourceManager(const ResourceManager& other)               = delete;
        ResourceManager& operator = (const ResourceManager& other)  = delete;
        ResourceManager(ResourceManager&& other)                    = delete;
        ResourceManager& operator = (ResourceManager&& other)       = delete;
    };






} }