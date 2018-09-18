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
        ArrayList<F32> m_segmentLengths;
        bool m_looped;

    public:
        CatmullRomSpline(const ArrayList<Math::Vec3>& controlPoints, bool looped = false);

        //----------------------------------------------------------------------
        void setControlPoints(const ArrayList<Math::Vec3>& controlPoints);

        //----------------------------------------------------------------------
        // @Return: Point of the splite at t.
        //----------------------------------------------------------------------
        Math::Vec3 getPoint(F32 t) const;

        //----------------------------------------------------------------------
        // @Return: Gradient of the spline at t.
        //----------------------------------------------------------------------
        Math::Vec3 getGradient(F32 t) const;

        //----------------------------------------------------------------------
        // @Return: Length of the nth segment.
        //----------------------------------------------------------------------
        F32 getSegmentLength(I32 node) const { return m_segmentLengths[node]; }

        //----------------------------------------------------------------------
        // @Return: Total length of the spline.
        //----------------------------------------------------------------------
        F32 getTotalLength() const;

        //----------------------------------------------------------------------
        // @Return: The normalised offset [0-PointCount] from a distance into the spline [0-splineDistance]
        //----------------------------------------------------------------------
        F32 getNormalisedOffset(F32 p) const;

    private:
        void _CalculateSegmentLengths();
    };

} // End namespaces
