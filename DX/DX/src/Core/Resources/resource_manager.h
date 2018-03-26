#pragma once
/**********************************************************************
    class: ResourceManager (resource_manager.h)

    author: S. Hau
    date: December 25, 2017

    Manages all resources in the game:
    - Stores them
    - Deletes them

    Ideas:
    Resource-Table in the background with a counter
    -> If counter reaches zero, delete resource (possibly with a delay e.g. after 5sec unused)
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "Graphics/i_mesh.hpp"
#include "Graphics/i_material.h"
#include "Graphics/i_shader.hpp"
#include "Graphics/i_texture.h"

namespace Core { namespace Resources {

    const static CString SHADER_DEFAULT_NAME    = "DEFAULT";
    const static CString SHADER_WIREFRAME_NAME  = "WIREFRAME";

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
        // Creates a new mesh resource
        //----------------------------------------------------------------------
        Graphics::Mesh* createMesh();
        Graphics::Mesh* createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices);
        Graphics::Mesh* createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices, const ArrayList<Math::Vec2>& uvs);

        //----------------------------------------------------------------------
        // Creates a new material resource
        // @Params:
        //  "shader": Shader to use for this material. If null default shader will be applied.
        //----------------------------------------------------------------------
        Graphics::Material* createMaterial(Graphics::Shader* shader = nullptr);

        //----------------------------------------------------------------------
        // Creates a new shader for use by the graphics engine.
        //----------------------------------------------------------------------
        Graphics::Shader* createShader(CString name, const OS::Path& vertPath, const OS::Path& fragPath);

        //----------------------------------------------------------------------
        // Creates a new texture resource
        // @Params:
        //  "width": Width of the texture in pixels
        //  "height": Height of the texture in pixels
        //  "format": The format of the texture
        //  "generateMips": If true a complete mipchain will be generated
        //----------------------------------------------------------------------
        Graphics::Texture* createTexture(U32 width, U32 height, Graphics::TextureFormat format, bool generateMips = true);

        //----------------------------------------------------------------------
        void UnloadUnusedResources();

        //----------------------------------------------------------------------
        U32 getMeshCount()      const { return static_cast<U32>( m_meshes.size() ); }
        U32 getShaderCount()    const { return static_cast<U32>( m_shaders.size() ); }
        U32 getMaterialCount()  const { return static_cast<U32>( m_materials.size() ); }
        U32 getTextureCount()   const { return static_cast<U32>( m_textures.size() ); }

        Graphics::Material*   getDefaultMaterial()    const { return m_defaultMaterial; }
        Graphics::Shader*     getDefaultShader()      const { return m_defaultShader; }

    private:
        ArrayList<Graphics::Mesh*>      m_meshes;
        ArrayList<Graphics::Shader*>    m_shaders;
        ArrayList<Graphics::Material*>  m_materials;
        ArrayList<Graphics::Texture*>   m_textures;

        HashMap<OS::Path, Graphics::Texture*> m_textureCache;

        Graphics::Shader*   m_defaultShader;
        Graphics::Shader*   m_wireframeShader;

        Graphics::Material* m_defaultMaterial;
        Graphics::Material* m_wireframeMaterial;

        //----------------------------------------------------------------------
        void _CreateDefaultAssets();

        //----------------------------------------------------------------------
        ResourceManager(const ResourceManager& other)               = delete;
        ResourceManager& operator = (const ResourceManager& other)  = delete;
        ResourceManager(ResourceManager&& other)                    = delete;
        ResourceManager& operator = (ResourceManager&& other)       = delete;
    };






} }