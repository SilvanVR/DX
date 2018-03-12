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

namespace Core { namespace Resources {

    //*********************************************************************
    class ResourceManager : public ISubSystem
    {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

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
        void UnloadUnusedResources();

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

    private:
        ArrayList<Graphics::Mesh*>      m_meshes;
        ArrayList<Graphics::Material*>  m_materials;

        //----------------------------------------------------------------------
        ResourceManager(const ResourceManager& other)               = delete;
        ResourceManager& operator = (const ResourceManager& other)  = delete;
        ResourceManager(ResourceManager&& other)                    = delete;
        ResourceManager& operator = (ResourceManager&& other)       = delete;
    };






} }