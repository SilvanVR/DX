#pragma once
/**********************************************************************
    class: Billboard (billboard.h)

    author: S. Hau
    date: May 23, 2018
**********************************************************************/

#include "i_component.h"
#include "Graphics/i_texture2d.hpp"

namespace Components {

    //**********************************************************************
    class Billboard : public Components::IComponent
    {
    public:
        Billboard(TexturePtr tex, F32 scale = 2.0f) 
            : m_billboardTexture(tex), m_scale(scale) {}

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;

    private:
        TexturePtr  m_billboardTexture  = nullptr;
        F32         m_scale             = 1.0f;
        GameObject* m_billboardGO       = nullptr;

        NULL_COPY_AND_ASSIGN(Billboard)
    };

}