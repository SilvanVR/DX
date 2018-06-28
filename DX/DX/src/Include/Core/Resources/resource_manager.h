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

#include "Common/i_subsystem.hpp"
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"
#include "Graphics/i_shader.h"
#include "Graphics/i_texture2d.hpp"
#include "Graphics/i_render_texture.h"
#include "Graphics/i_cubemap.hpp"
#include "Graphics/i_texture2d_array.hpp"
#include "Core/Audio/audio_clip.h"

namespace Core { namespace Resources {

    const static CString SHADER_DEFAULT_NAME        = "DEFAULT";
    const static CString SHADER_WIREFRAME_NAME      = "WIREFRAME";
    const static CString SHADER_ERROR_NAME          = "ERROR";
    const static CString SHADER_COLOR_NAME          = "COLOR";
    const static CString SHADER_POST_PROCESS_NAME   = "POSTPROCESS";
    const static CString SHADER_SHADOWMAP_NAME      = "SHADOWMAP";

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
        MaterialPtr createMaterial(const ShaderPtr& shader = nullptr);

        //----------------------------------------------------------------------
        // Creates a new shader
        //----------------------------------------------------------------------
        ShaderPtr createShader();

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
        // @Params:
        // "width/height": Width/Height of the texture in pixels.
        // "format": Texture format for the color buffer.
        // "depth": Depth-Format for the depth buffer.
        // "samplingDesc": Description for multisampling.
        // "dynamicScale": If true, render texture will be recreated with a new size whenever the window resizes.
        // "numBuffers": Amount of buffers to create. This is only useful if a camera renders itself, so a previous buffer can be used as input (e.g. for a mirror)
        //----------------------------------------------------------------------
        RenderTexturePtr createRenderTexture();
        RenderTexturePtr createRenderTexture(U32 width, U32 height, Graphics::TextureFormat format, bool dynamicScale);
        RenderTexturePtr createRenderTexture(U32 width, U32 height, Graphics::TextureFormat format, Graphics::SamplingDescription samplingDesc = {1, 0});
        RenderTexturePtr createRenderTexture(U32 width, U32 height, Graphics::DepthFormat depth, Graphics::TextureFormat format, U32 numBuffers = 1, 
                                             Graphics::MSAASamples sampleCount = Graphics::MSAASamples::One, bool dynamicScale = false);
        RenderTexturePtr createRenderTexture(U32 width, U32 height, Graphics::DepthFormat depth, Graphics::TextureFormat format,
                                             Graphics::MSAASamples sampleCount, bool dynamicScale);

        //----------------------------------------------------------------------
        // Creates a new render buffer
        //----------------------------------------------------------------------
        RenderBufferPtr createRenderBuffer();

        //----------------------------------------------------------------------
        // Creates a new cubemap
        //----------------------------------------------------------------------
        CubemapPtr createCubemap();

        //----------------------------------------------------------------------
        // Creates a new audio clip
        //----------------------------------------------------------------------
        AudioClipPtr createAudioClip();

        //----------------------------------------------------------------------
        // Sets the anisotropic filtering for all textures
        //----------------------------------------------------------------------
        void setGlobalAnisotropicFiltering(U32 level);

        //----------------------------------------------------------------------
        U32 getMeshCount()      const { return static_cast<U32>( m_meshes.size() ); }
        U32 getShaderCount()    const { return static_cast<U32>( m_shaders.size() ); }
        U32 getMaterialCount()  const { return static_cast<U32>( m_materials.size() ); }
        U32 getTextureCount()   const { return static_cast<U32>( m_textures.size() ); }

        const ArrayList<Graphics::Mesh*>&       getMeshes() const { return m_meshes; }
        const ArrayList<Graphics::Shader*>&     getShaders() const { return m_shaders; }
        const ArrayList<Graphics::Material*>&   getMaterials() const { return m_materials; }
        const ArrayList<Graphics::Texture*>&    getTextures() const { return m_textures; }
        const ArrayList<Audio::AudioClip*>&     getAudioClips() const { return m_audioClips; }

    private:
        ArrayList<Graphics::Mesh*>              m_meshes;
        ArrayList<Graphics::Shader*>            m_shaders;
        ArrayList<Graphics::Material*>          m_materials;
        ArrayList<Graphics::Texture*>           m_textures;
        ArrayList<Graphics::RenderTexture*>     m_renderTextures;
        ArrayList<Audio::AudioClip*>            m_audioClips;

        //----------------------------------------------------------------------
        void _DeleteTexture(Graphics::Texture* tex);
        void _DeleteRenderTexture(Graphics::RenderTexture* tex);
        void _DeleteMesh(Graphics::Mesh* mesh);
        void _DeleteMaterial(Graphics::Material* mat);
        void _DeleteShader(Graphics::Shader* shader);
        void _DeleteAudioClip(Audio::AudioClip* clip);

        void _OnWindowSizeChanged();

        NULL_COPY_AND_ASSIGN(ResourceManager)
    };






} }