#include "aabb.h"
/**********************************************************************
    class: AABB (aabb.cpp)

    author: S. Hau
    date: April 19, 2018
**********************************************************************/

namespace Math
{

    std::array<Math::Vec3, 8> AABB::getCorners() const
    {
        std::array<Math::Vec3, 8> corners;

        corners[0] = Math::Vec3( m_bounds[0].x, m_bounds[0].y, m_bounds[0].z );
        corners[1] = Math::Vec3( m_bounds[1].x, m_bounds[0].y, m_bounds[0].z );
        corners[2] = Math::Vec3( m_bounds[0].x, m_bounds[1].y, m_bounds[0].z );
        corners[3] = Math::Vec3( m_bounds[0].x, m_bounds[0].y, m_bounds[1].z );
        corners[4] = Math::Vec3( m_bounds[1].x, m_bounds[1].y, m_bounds[0].z );
        corners[5] = Math::Vec3( m_bounds[0].x, m_bounds[1].y, m_bounds[1].z );
        corners[6] = Math::Vec3( m_bounds[1].x, m_bounds[0].y, m_bounds[1].z );
        corners[7] = Math::Vec3( m_bounds[1].x, m_bounds[1].y, m_bounds[1].z );

        return corners;
    }

}