#pragma once
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/i_particle_system.h"

namespace Components {

    //**********************************************************************
    class ParticleSystem : public IRenderComponent
    {
    public:
        ParticleSystem(const ParticleSystemPtr& ps);

        //----------------------------------------------------------------------

    private:
        ParticleSystemPtr m_particleSystem;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(ParticleSystem)
    };

}