#pragma once
/**********************************************************************
    class: CModelRenderer (CModelRenderer.h)

    author: S. Hau
    date: December 19, 2017

    Used to render models.
**********************************************************************/

#include "../i_component.h"

namespace Components {

    class CModelRenderer : public IComponent
    {

    public:
        CModelRenderer() = default;

        // Model getModel() const { return m_model; }
        // void setModel(Model model) const { m_model = model;

        // Material getMaterial() const { return m_material; }
        // void setMesh(Material m) const { m_material = m; }

    private:
        // Model*           m_model;
        // Material*        m_material;


        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void recordGraphicsCommands(Core::Graphics::CommandBuffer& cmd, F32 lerp) override;


        //----------------------------------------------------------------------
        CModelRenderer(const CModelRenderer& other)               = delete;
        CModelRenderer& operator = (const CModelRenderer& other)  = delete;
        CModelRenderer(CModelRenderer&& other)                    = delete;
        CModelRenderer& operator = (CModelRenderer&& other)       = delete;
    };

}