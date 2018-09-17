#pragma once
/**********************************************************************
    class: Possibly several

    author: S. Hau
    date: September 19, 2018
**********************************************************************/

#include "dxmath_wrapper.h"

namespace Math {

    //**********************************************************************
    class CatmullRomSpline
    {
        ArrayList<Math::Vec3> m_points;
        bool m_looped;

    public:
        CatmullRomSpline(const ArrayList<Math::Vec3>& controlPoints, bool looped = false);

        //----------------------------------------------------------------------
        void setControlPoints(const ArrayList<Math::Vec3>& controlPoints);

        //----------------------------------------------------------------------
        // @Return: Point of the splite at t.
        //----------------------------------------------------------------------
        Math::Vec3 getSplinePoint(F32 t);

        //----------------------------------------------------------------------
        // @Return: Gradient of the spline at t.
        //----------------------------------------------------------------------
        Math::Vec3 getSplineGradient(F32 t);
    };

} // End namespaces
