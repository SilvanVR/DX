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
#include "Graphics/i_material.h"
#include "Graphics/i_shader.h"
#include "Graphics/i_mesh.h"

namespace Components {

    //**********************************************************************
    class Skybox : public Components::IComponent
    {
    public:
        Skybox(TexturePtr cubemap) : m_cubemap( cubemap ) {}

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;

        //----------------------------------------------------------------------
        void setCubemap(TexturePtr cubemap) { m_cubemap = cubemap; }
        const TexturePtr getCubemap() const { return m_cubemap; }

    private:
        TexturePtr  m_cubemap           = nullptr;

        ShaderPtr   m_skyboxShader      = nullptr;
        MaterialPtr m_skyboxMaterial    = nullptr;
        MeshPtr     m_skyboxMesh        = nullptr;

        //----------------------------------------------------------------------
        Skybox(const Skybox& other)               = delete;
        Skybox& operator = (const Skybox& other)  = delete;
        Skybox(Skybox&& other)                    = delete;
        Skybox& operator = (Skybox&& other)       = delete;
    };

}