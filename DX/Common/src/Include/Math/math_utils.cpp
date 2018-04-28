#pragma once
/**********************************************************************
    class: None (math_utils.cpp)
    
    author: S. Hau
    date: March 7, 2018
**********************************************************************/

#include "dxmath_wrapper.h"

namespace Math {

    //----------------------------------------------------------------------
    std::array<Vec3, 8> CalculateFrustumCorners( const Vec3& pos, const Vec3& up, const Vec3& right, const Vec3& forward,
                                                 F32 fovAngleYRad, F32 zNear, F32 zFar, F32 aspectRatio )
    {
        F32 halfTanFOV  = std::tanf( fovAngleYRad );

        F32 nearHeight  = 2.0f * halfTanFOV * zNear;
        F32 nearWidth   = nearHeight * aspectRatio;

        F32 farHeight   = 2.0f * halfTanFOV * zFar;
        F32 farWidth    = farHeight * aspectRatio;

        auto nearCenter = pos + forward * zNear;
        auto farCenter  = pos + forward * zFar;

        enum corner { NEAR_TOP_LEFT = 0, NEAR_TOP_RIGHT = 1, NEAR_BOTTOM_LEFT = 2, NEAR_BOTTOM_RIGHT = 3,
                      FAR_TOP_LEFT = 4, FAR_TOP_RIGHT = 5, FAR_BOTTOM_LEFT = 6, FAR_BOTTOM_RIGHT = 7 };

        std::array<Vec3, 8> vertices;
        vertices[NEAR_TOP_LEFT]     = nearCenter + up * (nearHeight*0.5f) - right * (nearWidth*0.5f);
        vertices[NEAR_TOP_RIGHT]    = nearCenter + up * (nearHeight*0.5f) + right * (nearWidth*0.5f);
        vertices[NEAR_BOTTOM_LEFT]  = nearCenter - up * (nearHeight*0.5f) - right * (nearWidth*0.5f);
        vertices[NEAR_BOTTOM_RIGHT] = nearCenter - up * (nearHeight*0.5f) + right * (nearWidth*0.5f);
        vertices[FAR_TOP_LEFT]      = farCenter + up * (farHeight*0.5f) - right * (farWidth*0.5f);
        vertices[FAR_TOP_RIGHT]     = farCenter + up * (farHeight*0.5f) + right * (farWidth*0.5f);
        vertices[FAR_BOTTOM_LEFT]   = farCenter - up * (farHeight*0.5f) - right * (farWidth*0.5f);
        vertices[FAR_BOTTOM_RIGHT]  = farCenter - up * (farHeight*0.5f) + right * (farWidth*0.5f);
        
        ArrayList<U32> indices = {
            NEAR_TOP_LEFT, NEAR_TOP_RIGHT,
            NEAR_BOTTOM_LEFT, NEAR_BOTTOM_RIGHT,
            NEAR_TOP_LEFT, NEAR_BOTTOM_LEFT,
            NEAR_TOP_RIGHT, NEAR_BOTTOM_RIGHT,
            FAR_TOP_LEFT, FAR_TOP_RIGHT,
            FAR_BOTTOM_LEFT, FAR_BOTTOM_RIGHT,
            FAR_TOP_LEFT, FAR_BOTTOM_LEFT,
            FAR_TOP_RIGHT, FAR_BOTTOM_RIGHT,
            NEAR_TOP_LEFT, FAR_TOP_LEFT,
            NEAR_TOP_RIGHT, FAR_TOP_RIGHT,
            NEAR_BOTTOM_LEFT, FAR_BOTTOM_LEFT,
            NEAR_BOTTOM_RIGHT, FAR_BOTTOM_RIGHT
        };

        return vertices;
    }

    //----------------------------------------------------------------------
    std::array<Vec3, 8> CalculateFrustumCorners(const Vec3& pos, const Quat& rot, F32 fovAngleYRad, F32 zNear, F32 zFar, F32 aspectRatio)
    {
        return CalculateFrustumCorners( pos, rot.getUp(), rot.getRight(), rot.getForward(), fovAngleYRad, zNear, zFar, aspectRatio );
    }

}