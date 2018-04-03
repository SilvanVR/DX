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
        Graphics::Mesh* createMesh();
        Graphics::Mesh* createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices);
        Graphics::Mesh* createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices, const ArrayList<Math::Vec2>& uvs);

        //----------------------------------------------------------------------
        // Creates a new material
        // @Params:
        //  "shader": Shader to use for this material. If null default shader will be applied.
        //----------------------------------------------------------------------
        Graphics::Material* createMaterial(Graphics::Shader* shader = nullptr);

        //----------------------------------------------------------------------
        // Creates a new shader
        //----------------------------------------------------------------------
        Graphics::Shader* createShader(CString name, const OS::Path& vertPath, const OS::Path& fragPath);

        //----------------------------------------------------------------------
        // Creates a new texture
        // @Params:
        //  "width": Width of the texture in pixels
        //  "height": Height of the texture in pixels
        //  "format": The format of the texture
        //  "generateMips": If true a complete mipchain will be generated
        //----------------------------------------------------------------------
        Graphics::Texture2D* createTexture2D(U32 width, U32 height, Graphics::TextureFormat format, bool generateMips = true);

        //----------------------------------------------------------------------
        // Creates a new immutable texture
        // @Params:
        //  "width": Width of the texture in pixels
        //  "height": Height of the texture in pixels
        //  "format": The format of the texture
        //  "pData": Pointer to initial texture data. If this is not null the
        //           texture will be immutable.
        //----------------------------------------------------------------------
        Graphics::Texture2D* createTexture2D(U32 width, U32 height, Graphics::TextureFormat format, const void* pData);

        //----------------------------------------------------------------------
        // Creates a new render texture
        //----------------------------------------------------------------------
        Graphics::RenderTexture* createRenderTexture();

        //----------------------------------------------------------------------
        // Creates a new cubemap
        //----------------------------------------------------------------------
        Graphics::Cubemap* createCubemap();

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

        Graphics::Material*   getDefaultMaterial()    const { return m_defaultMaterial; }
        Graphics::Shader*     getDefaultShader()      const { return m_defaultShader; }

        Graphics::Texture2D*  getBlackTexture() const { return m_black; }
        Graphics::Texture2D*  getWhiteTexture() const { return m_white; }

    private:
        ArrayList<Graphics::Mesh*>      m_meshes;
        ArrayList<Graphics::Shader*>    m_shaders;
        ArrayList<Graphics::Material*>  m_materials;
        ArrayList<Graphics::Texture*>   m_textures;

        Graphics::Shader*   m_defaultShader;
        Graphics::Shader*   m_errorShader;
        Graphics::Shader*   m_wireframeShader;

        Graphics::Material* m_defaultMaterial;
        Graphics::Material* m_wireframeMaterial;

        Graphics::Texture2D* m_black;
        Graphics::Texture2D* m_white;

        //----------------------------------------------------------------------
        void _CreateDefaultAssets();
        void _OnSceneChanged();

        //----------------------------------------------------------------------
        ResourceManager(const ResourceManager& other)               = delete;
        ResourceManager& operator = (const ResourceManager& other)  = delete;
        ResourceManager(ResourceManager&& other)                    = delete;
        ResourceManager& operator = (ResourceManager&& other)       = delete;
    };






} }