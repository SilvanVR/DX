#pragma once
/**********************************************************************
    class: Skeleton 

    author: S. Hau
    date: October 29, 2018
**********************************************************************/

namespace Animation { 

    const I32 MAX_BONE_WEIGHTS = 4;

    //----------------------------------------------------------------------
    struct SkeletonJoint
    {
        StringID name;
        I32 parentIndex;
        DirectX::XMMATRIX invBindPose;
    };

    //*********************************************************************
    struct Skeleton
    {
        ArrayList<SkeletonJoint> joints;
    };

    //----------------------------------------------------------------------
    // @Return: The level of the joint in the hierarchy
    //----------------------------------------------------------------------
    I32 JointLevel(const Skeleton& skeleton, const SkeletonJoint& joint);

} // End namespaces