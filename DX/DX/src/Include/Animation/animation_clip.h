#pragma once
/**********************************************************************
    class: Skeleton 

    author: S. Hau
    date: October 29, 2018
**********************************************************************/

#include "Time/durations.h"

namespace Animation { 

    //----------------------------------------------------------------------
    struct TranslationKey
    {
        Math::Vec3      translation;
        Time::Seconds   time;
    };

    //----------------------------------------------------------------------
    struct RotationKey
    {
        Math::Quat      rotation;
        Time::Seconds   time;
    };

    //----------------------------------------------------------------------
    struct ScalingKey
    {
        Math::Vec3      scale;
        Time::Seconds   time;
    };

    //----------------------------------------------------------------------
    struct JointSamples
    {
        StringID                    name;
        ArrayList<TranslationKey>   translationKeys;
        ArrayList<RotationKey>      rotationKeys;
        ArrayList<ScalingKey>       scalingKeys;
    };

    //*********************************************************************
    struct AnimationClip
    {
        StringID name;
        Time::Seconds duration;
        ArrayList<JointSamples> jointSamples;
    };

} // End namespaces