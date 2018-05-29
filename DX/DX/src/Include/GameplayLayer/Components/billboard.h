#pragma once
/**********************************************************************
    class: Billboard (billboard.h)

    author: S. Hau
    date: May 23, 2018
**********************************************************************/

#include "i_component.h"
#include "Graphics/i_material.h"
#include "Graphics/i_shader.h"
#include "Graphics/i_mesh.h"

namespace Components {

    //**********************************************************************
    class Billboard : public Components::IComponent
    {
    public:
        Billboard(TexturePtr tex, F32 scale = 1.0f) 
            : m_billboardTexture( tex ), m_scale( scale ) {}

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void tick(Time::Seconds delta) override;

    private:
        TexturePtr  m_billboardTexture  = nullptr;
        F32         m_scale             = 1.0f;
        ShaderPtr   m_billboardShader   = nullptr;
        MaterialPtr m_billboardMaterial = nullptr;
        MeshPtr     m_billboardMesh     = nullptr;
        GameObject* m_billboardGO       = nullptr;

        //----------------------------------------------------------------------
        Billboard(const Billboard& other)               = delete;
        Billboard& operator = (const Billboard& other)  = delete;
        Billboard(Billboard&& other)                    = delete;
        Billboard& operator = (Billboard&& other)       = delete;
    };

}