#pragma once
/**********************************************************************
    class: MeshRenderer (mesh_renderer.h)

    author: S. Hau
    date: December 19, 2017

    Used to render models.
    A Mesh-Renderer consists of a mesh (with possibly submeshes) and
    a list of materials. Each material belongs to the corresponding
    submesh in the mesh-class e.g. SubMesh 1 gets rendered with Material 1.
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/i_mesh.hpp"
#include "Graphics/i_material.hpp"

namespace Components {

    //**********************************************************************
    class MeshRenderer : public IRenderComponent
    {
    public:
        MeshRenderer() = default;
        MeshRenderer(Graphics::Mesh* mesh, Graphics::Material* material = nullptr);

        Graphics::Mesh*                         getMesh()                   const   { return m_mesh; }
        Graphics::Material*                     getMaterial(U32 index = 0)  const   { return m_materials[index]; }
        const ArrayList<Graphics::Material*>&   getMaterials()              const   { return m_materials; }
        U32                                     getMaterialCount()          const   { return static_cast<U32>( m_materials.size() ); }

        void setMesh(Graphics::Mesh* mesh);
        void setMaterial(Graphics::Material* m, U32 index = 0);

    private:
        Graphics::Mesh*                 m_mesh;
        ArrayList<Graphics::Material*>  m_materials;

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void AddedToGameObject(GameObject* go) override;
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;


        //----------------------------------------------------------------------
        MeshRenderer(const MeshRenderer& other)               = delete;
        MeshRenderer& operator = (const MeshRenderer& other)  = delete;
        MeshRenderer(MeshRenderer&& other)                    = delete;
        MeshRenderer& operator = (MeshRenderer&& other)       = delete;
    };

}