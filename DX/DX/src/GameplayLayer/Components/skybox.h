#pragma once
/**********************************************************************
    class: Skybox (skybox.h)

    author: S. Hau
    date: April 1, 2018

    Component which renders a skybox in the background after all opaque
    geometry has been rendered.
**********************************************************************/

#include "i_component.h"
#include "Graphics/i_cubemap.hpp"

namespace Components {

    //**********************************************************************
    class Skybox : public Components::IComponent
    {
    public:
        Skybox(Graphics::Cubemap* cubemap) : m_cubemap( cubemap ) {}

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void AddedToGameObject(GameObject* go) override;

        //----------------------------------------------------------------------
        void setCubemap(Graphics::Cubemap* cubemap) { m_cubemap = cubemap; }
        const Graphics::Cubemap* getCubemap() const { return m_cubemap; }

    private:
        Graphics::Cubemap* m_cubemap = nullptr;

        //----------------------------------------------------------------------
        Skybox(const Skybox& other)               = delete;
        Skybox& operator = (const Skybox& other)  = delete;
        Skybox(Skybox&& other)                    = delete;
        Skybox& operator = (Skybox&& other)       = delete;
    };

}