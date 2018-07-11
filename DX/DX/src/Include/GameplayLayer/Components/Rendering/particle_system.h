#pragma once
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"

namespace Components {

    //**********************************************************************
    class ParticleSystem : public IRenderComponent
    {
    public:
        ParticleSystem(const MaterialPtr& material);

        //----------------------------------------------------------------------
        void tick(Time::Seconds delta) override;

    private:
        MeshPtr     m_particleSystem;
        MaterialPtr m_material;
        U32         m_maxParticles = 100;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(ParticleSystem)
    };

}