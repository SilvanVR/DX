#pragma once
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "Math/aabb.h"

namespace Graphics {

    class IShader;
    class VertexLayout;

    //**********************************************************************
    class IParticleSystem
    {
    public:
        IParticleSystem() = default;
        virtual ~IParticleSystem() {}

        //----------------------------------------------------------------------
        const Math::AABB& getBounds() const { return m_bounds; }

    protected:
        Math::AABB  m_bounds;

    private:
        //----------------------------------------------------------------------
        // Binds this particle system to the pipeline. Subsequent drawcalls renders it.
        //----------------------------------------------------------------------
        virtual void bind(const VertexLayout& vertLayout, U32 subMesh = 0) = 0;

        //----------------------------------------------------------------------
        // Recalculate the AABB for this particle system
        //----------------------------------------------------------------------
        void _RecalculateBounds();

        NULL_COPY_AND_ASSIGN(IParticleSystem)
    };

    using ParticleSystem = IParticleSystem;

} // End namespaces

using ParticleSystemPtr = std::shared_ptr<Graphics::IParticleSystem>;