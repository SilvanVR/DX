#pragma once
/**********************************************************************
    class: Several (dxmath_wrapper.h)
    
    author: S. Hau
    date: March 3, 2018

    Wraps some functionality from <DirectXMath> in custom classes, 
    so its easier to use and the code looks cleaner.
**********************************************************************/

#ifdef _WIN32

#include <DirectXMath.h>

namespace Math {

    //**********************************************************************
    class Vector3F : public DirectX::XMFLOAT3
    {
    public:
        Vector3F(F32 value = 0);
        Vector3F(F32 x, F32 y, F32 z);

        Vector3F    operator +      (const Vector3F& v) const { return Vector3F( x + v.x, y + v.y, z + v.z ); }
        Vector3F    operator -      (const Vector3F& v) const { return Vector3F( x - v.x, y - v.y, z - v.z ); }
        Vector3F    operator *      (F32 s)             const { return Vector3F( x * s, y * s, z * s ); }
        Vector3F    operator /      (F32 s)             const { return Vector3F(x / s, y / s, z / s); }
        Vector3F&   operator +=     (const Vector3F& v) { x += v.x, y += v.y; z += v.z; return *this; }
        Vector3F&   operator -=     (const Vector3F& v) { x -= v.x, y -= v.y; z -= v.z; return *this; }
        Vector3F&   operator *=     (F32 s)             { x *= s; y *= s; z *= s; return *this; }
        Vector3F&   operator /=     (F32 s)             { x /= s; y /= s; z /= s; return *this; }

        F32         magnitude() const;
        Vector3F    normalized() const;
        Vector3F    cross(const Vector3F& v) const;

        String toString() const { return "(" + TS(x) + "," + TS(y) + "," + TS(z) + ")"; }

        // Static members
        static const Vector3F UP;            //same as ( 0, 1, 0 )
        static const Vector3F DOWN;          //same as ( 0,-1, 0 )
        static const Vector3F LEFT;          //same as (-1, 0, 0 )
        static const Vector3F RIGHT;         //same as ( 1, 0, 0 )
        static const Vector3F FORWARD;       //same as ( 0, 0, 1 )
        static const Vector3F BACK;          //same as ( 0, 0,-1 )
        static const Vector3F ONE;           //same as ( 1, 1, 1 )
        static const Vector3F ZERO;          //same as ( 0, 0, 0 )
    };

    //**********************************************************************
    class Vector4F : public DirectX::XMFLOAT4
    {
    public:
        Vector4F(F32 value = 0);
        Vector4F(F32 x, F32 y, F32 z, F32 w);

        Vector4F    operator +      (const Vector4F& v) const { return Vector4F( x + v.x, y + v.y, z + v.z, w + v.w ); }
        Vector4F    operator -      (const Vector4F& v) const { return Vector4F( x - v.x, y - v.y, z - v.z, w - v.w ); }
        Vector4F&   operator +=     (const Vector4F& v) { x += v.x, y += v.y; z += v.z; w += v.w; return *this; }
        Vector4F&   operator -=     (const Vector4F& v) { x -= v.x, y -= v.y; z -= v.z; w -= v.w; return *this; }

        String toString() const { return "(" + TS(x) + "," + TS(y) + "," + TS(z) + "," +TS(w) + ")"; }
    };

    //**********************************************************************
    class Quaternion : public DirectX::XMFLOAT4
    {
    public:
        Quaternion(F32 x = 0, F32 y = 0, F32 z = 0, F32 w = 1);
        Quaternion(const Vector3F& axis, F32 angleInDegrees);

        Quaternion  operator *  (const Quaternion& q)    const;
        Vector3F    operator *  (const Vector3F& v)      const;
        Quaternion& operator *= (const Quaternion& q);

        F32         magnitude() const;
        Quaternion  normalized() const;

        Vector3F getForward()   const { return *this * Vector3F::FORWARD; }
        Vector3F getBack()      const { return *this * Vector3F::BACK; }
        Vector3F getLeft()      const { return *this * Vector3F::LEFT; }
        Vector3F getRight()     const { return *this * Vector3F::RIGHT; }
        Vector3F getUp()        const { return *this * Vector3F::UP; }
        Vector3F getDown()      const { return *this * Vector3F::DOWN; }

        static Quaternion LookRotation(const Vector3F& forward, const Vector3F& up);
        static Quaternion FromEulerAngles(F32 pitchDegrees, F32 yawDegrees, F32 rollDegrees);
        static Quaternion FromEulerAngles(const Vector3F& eulerAnglesInDegrees);

        String toString() const { return "(" + TS(x) + "," + TS(y) + "," + TS(z) + "," + TS(w) + ")"; }

        // Static members
        static const Quaternion IDENTITY;
    };

    //----------------------------------------------------------------------
    using Vec3 = Vector3F;
    using Vec4 = Vector4F;
    using Quat = Quaternion;

}





#else
    static_assert("DX Math-Wrapper not supported on this plattform!");
#endif