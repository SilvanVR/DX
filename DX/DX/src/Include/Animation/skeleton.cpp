#include "skeleton.h"
/**********************************************************************
    class: Skeleton 

    author: S. Hau
    date: October 29, 2018
**********************************************************************/

namespace Animation { 

    //----------------------------------------------------------------------
    I32 JointLevel( const Skeleton& skeleton, const SkeletonJoint& joint )
    {
        I32 level = 0;
        const Animation::SkeletonJoint* curJoint = &joint;
        while (curJoint->parentIndex >= 0)
        {
            curJoint = &skeleton.joints[curJoint->parentIndex];
            level++;
        }
        return level;
    }

} // End namespaces