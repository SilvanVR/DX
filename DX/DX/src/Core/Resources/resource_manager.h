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
#include "Graphics/i_material.hpp"
#include "Graphics/i_shader.hpp"

namespace Core { namespace Resources {

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
        //----------------------------------------------------------------------
        Graphics::Material* createMaterial();

        //----------------------------------------------------------------------
        // Creates a new shader for use by the graphics engine.
        //----------------------------------------------------------------------
        Graphics::Shader* createShader(CString vertPath, CString fragPath);

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
        Graphics::Material* m_defaultMaterial;

        //----------------------------------------------------------------------
        ResourceManager(const ResourceManager& other)               = delete;
        ResourceManager& operator = (const ResourceManager& other)  = delete;
        ResourceManager(ResourceManager&& other)                    = delete;
        ResourceManager& operator = (ResourceManager&& other)       = delete;
    };






} }