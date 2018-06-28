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
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"

namespace Components {

    //**********************************************************************
    class MeshRenderer : public IRenderComponent
    {
    public:
        MeshRenderer() = default;
        MeshRenderer(const MeshPtr& mesh, const MaterialPtr& material = nullptr);

        const MeshPtr&                          getMesh()                   const   { return m_mesh; }
        const MaterialPtr&                      getMaterial(U32 index = 0)  const   { return m_materials[index]; }
        const ArrayList<MaterialPtr>&           getMaterials()              const   { return m_materials; }
        U32                                     getMaterialCount()          const   { return static_cast<U32>( m_materials.size() ); }

        //----------------------------------------------------------------------
        // Set the mesh in this mesh renderer component.
        //----------------------------------------------------------------------
        void setMesh(const MeshPtr& mesh);

        //----------------------------------------------------------------------
        // Set a material in this mesh renderer component.
        // @Params:
        //  "m": The material.
        //  "subMeshIndex": The index of the submesh, which gains the material.
        //----------------------------------------------------------------------
        void setMaterial(const MaterialPtr& m, U32 subMeshIndex = 0);

    private:
        MeshPtr                 m_mesh;
        ArrayList<MaterialPtr>  m_materials;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        void recordGraphicsCommandsShadows(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(MeshRenderer)
    };

}