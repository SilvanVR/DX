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
        // Creates a new mesh for use by the graphics engine.
        //----------------------------------------------------------------------
        Graphics::Mesh* createMesh();
        Graphics::Mesh* createMesh(const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices);

        //----------------------------------------------------------------------
        // Creates a new material for use by the graphics engine.
        // @Params:
        //  "shader": Shader to use for this material. If null default shader will be applied.
        //----------------------------------------------------------------------
        Graphics::Material* createMaterial(Graphics::Shader* shader = nullptr);

        //----------------------------------------------------------------------
        // Creates a new shader for use by the graphics engine.
        //----------------------------------------------------------------------
        Graphics::Shader* createShader(CString name, const OS::Path& vertPath, const OS::Path& fragPath);

        //----------------------------------------------------------------------
        void UnloadUnusedResources();

        //----------------------------------------------------------------------
        U32 getMeshCount()      const { return static_cast<U32>( m_meshes.size() ); }
        U32 getShaderCount()    const { return static_cast<U32>( m_shaders.size() ); }
        U32 getMaterialCount()  const { return static_cast<U32>( m_materials.size() ); }

        Graphics::Material*   getDefaultMaterial()    const { return m_defaultMaterial; }
        Graphics::Shader*     getDefaultShader()      const { return m_defaultShader; }

    private:
        ArrayList<Graphics::Mesh*>      m_meshes;
        ArrayList<Graphics::Shader*>    m_shaders;
        ArrayList<Graphics::Material*>  m_materials;

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