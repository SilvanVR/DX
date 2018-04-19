#pragma once
/**********************************************************************
    class: Ray (ray.h)

    author: S. Hau
    date: April 19, 2018
**********************************************************************/

#include "Math/aabb.h"

namespace Physics
{
    //----------------------------------------------------------------------
    struct RayCastResult
    {
        Math::Vec3 hitPoint = Math::Vec3(0);
    };

    //**********************************************************************
    class Ray
    {
    public:
        Ray(const Math::Vec3& orig, const Math::Vec3& dir);

        const Math::Vec3& getOrigin()       const { return m_origin; }
        const Math::Vec3& getDirection()    const { return m_direction; }
        const Math::Vec3& getInvDir()       const { return m_invdir; }

        //----------------------------------------------------------------------
        // Computes whether this ray intersects the given AABB
        // @Return:
        //  True when the ray intersects the given AABB.
        //----------------------------------------------------------------------
        bool intersects(const Math::AABB& bb) const;

        //----------------------------------------------------------------------
        // Computes whether this ray intersects the given AABB
        // @Return:
        //  True when the ray intersects the given AABB and stores the distance
        //  from the rays origin to the hitpoint. If not interested in the distance
        //  use the other functions which is faster.
        //----------------------------------------------------------------------
        bool intersects(const Math::AABB& bb, F32* distance) const;
        bool intersects(const Math::AABB& bb, RayCastResult* result) const;

    private:
        Math::Vec3  m_origin;
        Math::Vec3  m_direction;
        Math::Vec3  m_invdir;
        I32         m_sign[3];
    };


}