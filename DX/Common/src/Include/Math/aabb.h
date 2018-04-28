#pragma once
/**********************************************************************
    class: AABB (aabb.h)

    author: S. Hau
    date: April 19, 2018

    Axis aligned bounding box.
**********************************************************************/

#include "dxmath_wrapper.h"
#include <array>

namespace Math
{

    //**********************************************************************
    class AABB
    {
        Math::Vec3 m_bounds[2];

    public:
        AABB()
            : m_bounds{ Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 0) }
        {}

        AABB(const Math::Vec3& min, const Math::Vec3& max)
            : m_bounds{ min, max }
        {}

        const Math::Vec3& getMin() const { return m_bounds[0]; }
        const Math::Vec3& getMax() const { return m_bounds[1]; }

        Math::Vec3& getMin() { return m_bounds[0]; }
        Math::Vec3& getMax() { return m_bounds[1]; }

        std::array<Math::Vec3, 8> getCorners() const;

        const Math::Vec3&   operator[] (I32 index) const    { ASSERT( index < 2 ); return m_bounds[index]; }
        Math::Vec3&         operator[] (I32 index)          { ASSERT( index < 2 ); return m_bounds[index]; }

        String toString() { return "Min: " + getMin().toString() + " Max: " + getMax().toString(); }
    };

}