#pragma once
/**********************************************************************
    class: None (math_utils.h)
    
    author: S. Hau
    date: March 7, 2018

    Common utility functions for math stuff.
**********************************************************************/

#include "dxmath_wrapper.h"
#include <array>

namespace Math {

    //----------------------------------------------------------------------
    template <typename T> inline
    float Deg2Rad( T val )
    {
        return val * 0.01745329251f;
    }

    //----------------------------------------------------------------------
    template <typename T> inline
    float Rad2Deg( T val )
    {
        return val * 57.2957795131f;
    }

    //----------------------------------------------------------------------
    std::array<Vec3, 8> CalculateFrustumCorners(const Vec3& pos, const Vec3& up, const Vec3& right, const Vec3& forward,
                                                F32 fovAngleYRad, F32 zNear, F32 zFar, F32 aspectRatio);
    std::array<Vec3, 8> CalculateFrustumCorners(const Vec3& pos, const Quat& rot, F32 fovAngleYRad, F32 zNear, F32 zFar, F32 aspectRatio);

    //----------------------------------------------------------------------
    template <typename T, typename T2, typename T3> inline
    T Clamp( T val, T2 min, T3 max )
    {
        if (val < min)
            return min;
        else if (val > max)
            return max;
        return val;
    }

    //----------------------------------------------------------------------
    template <typename T, typename T2, typename T3> inline
    T ClampAngle( T angle, T2 min, T3 max )
    {
        if (angle < -360)
            angle += 360;
        if (angle > 360)
            angle -= 360;
        return clamp( angle, min, max );
    }

    //----------------------------------------------------------------------
    template <typename T, typename T2>
    T Lerp( T a, T b, T2 t)
    {
        return a * (static_cast<T2>(1) - t) + b * t;
    }

}