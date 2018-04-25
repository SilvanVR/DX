#pragma once
/**********************************************************************
    class: ResourceManager (resource_manager.h)

    author: S. Hau
    date: December 25, 2017

    Manages all resources in the game:
    - Creates & knows about them
    - Deletes them

    This is to separate the concept of creating/deleting resources
    and loading them from disk, which is the responsibility of the
    AssetManager.
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"
#include "Graphics/i_shader.hpp"
#include "Graphics/i_texture2d.hpp"
#include "Graphics/i_render_texture.hpp"
#include "Graphics/i_cubemap.hpp"
#include "Graphics/i_texture2d_array.hpp"
#include "Core/Audio/audio_clip.h"

namespace Core { namespace Resources {

    const static CString SHADER_DEFAULT_NAME    = "DEFAULT";
    const static CString SHADER_WIREFRAME_NAME  = "WIREFRAME";
    const static CString SHADER_ERROR_NAME      = "ERROR";
    const static CString SHADER_COLOR_NAME      = "COLOR";

    //*********************************************************************
    class ResourceManager : public ISubSystem
    {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
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
        ShaderPtr createShader(const String& vertSrc, const String& fragSrc);

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
        // Creates a new audio clip
        //----------------------------------------------------------------------
        AudioClipPtr createAudioClip();

        //----------------------------------------------------------------------
        // Sets the anisotropic filtering for all textures @TODO: move this somewhere else
        //----------------------------------------------------------------------
        void setGlobalAnisotropicFiltering(U32 level);

        //----------------------------------------------------------------------
        U32 getMeshCount()      const { return static_cast<U32>( m_meshes.size() ); }
        U32 getShaderCount()    const { return static_cast<U32>( m_shaders.size() ); }
        U32 getMaterialCount()  const { return static_cast<U32>( m_materials.size() ); }
        U32 getTextureCount()   const { return static_cast<U32>( m_textures.size() ); }

        const MaterialPtr&      getDefaultMaterial() const { return m_defaultMaterial; }
        const ShaderPtr&        getDefaultShader()   const { return m_defaultShader; }
        const ShaderPtr&        getColorShader() const { return m_colorShader; }
        const MaterialPtr&      getColorMaterial() const { return m_colorMaterial; }

        const Texture2DPtr&     getBlackTexture() const { return m_black; }
        const Texture2DPtr&     getWhiteTexture() const { return m_white; }

        const CubemapPtr&       getDefaultCubemap() const { return m_defaultCubemap; }

    private:
        ArrayList<Graphics::Mesh*>      m_meshes;
        ArrayList<Graphics::Shader*>    m_shaders;
        ArrayList<Graphics::Material*>  m_materials;
        ArrayList<Graphics::Texture*>   m_textures;
        ArrayList<Audio::AudioClip*>    m_audioClips;

        ShaderPtr       m_defaultShader;
        ShaderPtr       m_errorShader;
        ShaderPtr       m_wireframeShader;
        ShaderPtr       m_colorShader;

        MaterialPtr     m_defaultMaterial;
        MaterialPtr     m_wireframeMaterial;
        MaterialPtr     m_colorMaterial;

        Texture2DPtr    m_black;
        Texture2DPtr    m_white;

        CubemapPtr      m_defaultCubemap;

        //----------------------------------------------------------------------
        void _CreateDefaultAssets();

        void _DeleteTexture(Graphics::Texture* tex);
        void _DeleteMesh(Graphics::Mesh* mesh);
        void _DeleteMaterial(Graphics::Material* mat);
        void _DeleteShader(Graphics::Shader* shader);
        void _DeleteAudioClip(Audio::AudioClip* clip);

        //----------------------------------------------------------------------
        ResourceManager(const ResourceManager& other)               = delete;
        ResourceManager& operator = (const ResourceManager& other)  = delete;
        ResourceManager(ResourceManager&& other)                    = delete;
        ResourceManager& operator = (ResourceManager&& other)       = delete;
    };






} }