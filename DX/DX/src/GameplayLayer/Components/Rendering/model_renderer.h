#pragma once
/**********************************************************************
    class: ModelRenderer (model_renderer.h)

    author: S. Hau
    date: December 19, 2017

    Used to render models.
**********************************************************************/

#include "i_render_component.hpp"
#include "Assets/model.h"

namespace Components {

    //**********************************************************************
    class ModelRenderer : public IRenderComponent
    {
    public:
        ModelRenderer() = default;

        Assets::Model*  getModel()                      const   { return m_model; }
        void            setModel(Assets::Model* model)          { m_model = model; }

        // Material getMaterial() const { return m_material; }
        // void setMesh(Material m) const { m_material = m; }

    private:
        Assets::Model*      m_model;
        // Material*        m_material;


        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;


        //----------------------------------------------------------------------
        ModelRenderer(const ModelRenderer& other)               = delete;
        ModelRenderer& operator = (const ModelRenderer& other)  = delete;
        ModelRenderer(ModelRenderer&& other)                    = delete;
        ModelRenderer& operator = (ModelRenderer&& other)       = delete;
    };

}