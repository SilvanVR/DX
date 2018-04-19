#include "ray.h"
/**********************************************************************
    class: Ray (ray.cpp)

    author: S. Hau
    date: April 19, 2018
**********************************************************************/

namespace Physics
{

    //----------------------------------------------------------------------
        Ray::Ray( const Math::Vec3& origin, const Math::Vec3& direction ) 
            : m_origin( origin ), m_direction( direction )
        {
            m_invdir = Math::Vec3( 1.0f / m_direction.x, 1.0f / m_direction.y, 1.0f / m_direction.z );
            m_sign[0] = ( m_invdir.x < 0 );
            m_sign[1] = ( m_invdir.y < 0 );
            m_sign[2] = ( m_invdir.z < 0 );
        }

        //----------------------------------------------------------------------
        bool Ray::intersects(const Math::AABB& bb) const
        {
            F64 t1 = ( bb.getMin()[0] - m_origin[0] ) * m_invdir[0];
            F64 t2 = ( bb.getMax()[0] - m_origin[0] ) * m_invdir[0];

            F64 tmin = std::min( t1, t2 );
            F64 tmax = std::max( t1, t2 );

            for (int i = 1; i < 3; ++i) 
            {
                t1 = ( bb.getMin()[i] - m_origin[i] ) * m_invdir[i];
                t2 = ( bb.getMax()[i] - m_origin[i] ) * m_invdir[i];

                tmin = std::max( tmin, std::min( t1, t2 ) );
                tmax = std::min( tmax, std::max( t1, t2 ) );
            }

            return tmax > std::max( tmin, 0.0 );
        }

        //----------------------------------------------------------------------
        bool Ray::intersects( const Math::AABB& bb, F32* distance ) const
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            tmin  = ( bb[ m_sign[0]     ].x - m_origin.x) * m_invdir.x;
            tmax  = ( bb[ 1 - m_sign[0] ].x - m_origin.x) * m_invdir.x;
            tymin = ( bb[ m_sign[1]     ].y - m_origin.y) * m_invdir.y;
            tymax = ( bb[ 1 - m_sign[1] ].y - m_origin.y) * m_invdir.y;

            if ( (tmin > tymax) || (tymin > tmax) )
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = ( bb[ m_sign[2]     ].z - m_origin.z) * m_invdir.z;
            tzmax = ( bb[ 1 - m_sign[2] ].z - m_origin.z) * m_invdir.z;

            if ( (tmin > tzmax) || (tzmin > tmax) )
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            *distance = tmin;

            if (*distance < 0) {
                *distance = tmax;
                if (*distance < 0) return false;
            }

            return true;
        }

        //----------------------------------------------------------------------
        bool Ray::intersects( const Math::AABB& bb, RayCastResult* result ) const
        {
            F32 t = 0;
            bool hit = intersects( bb, &t );

            if (hit)
                result->hitPoint = m_origin + m_direction * t;

            return hit;
        }
}