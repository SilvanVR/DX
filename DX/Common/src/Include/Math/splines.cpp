#include "splines.h"
/**********************************************************************
    class: Possibly several

    author: S. Hau
    date: September 19, 2018
**********************************************************************/

namespace Math
{

    //----------------------------------------------------------------------
    CatmullRomSpline::CatmullRomSpline( const ArrayList<Math::Vec3>& controlPoints, bool looped )
        : m_looped( looped )
    {
        setControlPoints( controlPoints );
    }

    //----------------------------------------------------------------------
    void CatmullRomSpline::setControlPoints( const ArrayList<Math::Vec3>& controlPoints )
    {
        m_points = controlPoints;
        ASSERT( m_points.size() >= 4 && "Min. 4 points required!" );
    }

    //----------------------------------------------------------------------
    Math::Vec3 CatmullRomSpline::getSplinePoint( F32 t )
    {
        I32 p0, p1, p2, p3;
        if (not m_looped)
        {
            p1 = I32(t) + 1;
            p2 = p1 + 1;
            p3 = p2 + 1;
            p0 = p1 - 1;
        }
        else
        {
            p1 = I32(t);
            p2 = (p1 + 1) % (I32)m_points.size();
            p3 = (p2 + 1) % (I32)m_points.size();
            p0 = p1 >= 1 ? p1 - 1 : (I32)m_points.size() - 1;
        }

        t = t - (I32)t;

        F32 tt = t * t;
        F32 ttt = tt * t;

        F32 q1 = -ttt + 2.0f*tt - t;
        F32 q2 = 3.0f*ttt - 5.0f*tt + 2.0f;
        F32 q3 = -3.0f*ttt + 4.0f*tt + t;
        F32 q4 = ttt - tt;

        F32 tx = m_points[p0].x * q1 + m_points[p1].x * q2 + m_points[p2].x * q3 + m_points[p3].x * q4;
        F32 ty = m_points[p0].y * q1 + m_points[p1].y * q2 + m_points[p2].y * q3 + m_points[p3].y * q4;
        F32 tz = m_points[p0].z * q1 + m_points[p1].z * q2 + m_points[p2].z * q3 + m_points[p3].z * q4;

        return { 0.5f*tx, 0.5f*ty, 0.5f * tz };
    }

    //----------------------------------------------------------------------
    Math::Vec3 CatmullRomSpline::getSplineGradient( F32 t )
    {
        I32 p0, p1, p2, p3;
        if (not m_looped)
        {
            p1 = I32(t) + 1;
            p2 = p1 + 1;
            p3 = p2 + 1;
            p0 = p1 - 1;
        }
        else
        {
            p1 = I32(t);
            p2 = (p1 + 1) % (I32)m_points.size();
            p3 = (p2 + 1) % (I32)m_points.size();
            p0 = p1 >= 1 ? p1 - 1 : (I32)m_points.size() - 1;
        }

        t = t - (I32)t;

        F32 tt = t * t;
        F32 ttt = tt * t;

        F32 q1 = -3.0f*tt + 4.0f*t - 1;
        F32 q2 = 9.0f*tt - 10.0f*t;
        F32 q3 = -9.0f*tt + 8.0f*t + 1.0f;
        F32 q4 = 3.0f*tt - 2.0f*t;

        F32 tx = m_points[p0].x * q1 + m_points[p1].x * q2 + m_points[p2].x * q3 + m_points[p3].x * q4;
        F32 ty = m_points[p0].y * q1 + m_points[p1].y * q2 + m_points[p2].y * q3 + m_points[p3].y * q4;
        F32 tz = m_points[p0].z * q1 + m_points[p1].z * q2 + m_points[p2].z * q3 + m_points[p3].z * q4;

        return { 0.5f*tx, 0.5f*ty, 0.5f*tz };
    }

} // End namespaces
