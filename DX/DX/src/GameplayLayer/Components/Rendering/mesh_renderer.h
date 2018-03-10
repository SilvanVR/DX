#pragma once
/**********************************************************************
    class: MeshRenderer (mesh_renderer.h)

    author: S. Hau
    date: December 19, 2017

    Used to render models.
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/i_mesh.hpp"

namespace Components {

    //**********************************************************************
    class MeshRenderer : public IRenderComponent
    {
    public:
        MeshRenderer() = default;
        MeshRenderer(Graphics::Mesh* mesh) : m_mesh( mesh ) {}

        Graphics::Mesh* getMesh()                      const   { return m_mesh; }
        void            setMesh(Graphics::Mesh* mesh)          { m_mesh = mesh; }

        // Material getMaterial() const { return m_material; }
        // void setMaterial(Material m) const { m_material = m; }

    private:
        Graphics::Mesh*     m_mesh;
        // Material*        m_material;


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