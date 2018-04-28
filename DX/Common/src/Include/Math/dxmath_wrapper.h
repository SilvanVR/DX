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
    class Vector2F : public DirectX::XMFLOAT2
    {
    public:
        Vector2F(F32 value = 0);
        Vector2F(F32 x, F32 y);

        Vector2F    operator +      (const Vector2F& v) const { return Vector2F( x + v.x, y + v.y ); }
        Vector2F    operator -      (const Vector2F& v) const { return Vector2F( x - v.x, y - v.y ); }
        Vector2F    operator *      (F32 s)             const { return Vector2F( x * s, y * s ); }
        Vector2F    operator /      (F32 s)             const { return Vector2F( x / s, y / s); }
        Vector2F&   operator +=     (const Vector2F& v) { x += v.x, y += v.y; return *this; }
        Vector2F&   operator -=     (const Vector2F& v) { x -= v.x, y -= v.y; return *this; }
        Vector2F&   operator *=     (F32 s)             { x *= s; y *= s; return *this; }
        Vector2F&   operator /=     (F32 s)             { x /= s; y /= s; return *this; }

        bool        operator == (const Vector2F& v) const { return x == v.x && y == v.y; }
        bool        operator != (const Vector2F& v) const { return !(*this == v); }

        F32         magnitude() const;
        Vector2F    normalized() const;

        String toString() const { return "(" + TS(x) + "," + TS(y) +  ")"; }

        // Static members
        static const Vector2F UP;            //same as ( 0, 1,)
        static const Vector2F DOWN;          //same as ( 0,-1,)
        static const Vector2F LEFT;          //same as (-1, 0,)
        static const Vector2F RIGHT;         //same as ( 1, 0,)
        static const Vector2F ONE;           //same as ( 1, 1,)
        static const Vector2F ZERO;          //same as ( 0, 0,)
    };

    //**********************************************************************
    class Vector2Int : public DirectX::XMINT2
    {
    public:
        Vector2Int(I32 value = 0);
        Vector2Int(I32 x, I32 y);

        Vector2Int  operator +      (const Vector2Int& v) const { return Vector2Int( x + v.x, y + v.y ); }
        Vector2Int  operator -      (const Vector2Int& v) const { return Vector2Int( x - v.x, y - v.y ); }
        Vector2Int  operator *      (I32 s)               const { return Vector2Int( x * s, y * s ); }
        Vector2Int  operator /      (I32 s)               const { return Vector2Int( x / s, y / s ); }
        Vector2Int& operator +=     (const Vector2Int& v) { x += v.x, y += v.y; return *this; }
        Vector2Int& operator -=     (const Vector2Int& v) { x -= v.x, y -= v.y; return *this; }
        Vector2Int& operator *=     (I32 s) { x *= s; y *= s; return *this; }
        Vector2Int& operator /=     (I32 s) { x /= s; y /= s; return *this; }

        bool        operator == (const Vector2Int& v) const { return x == v.x && y == v.y; }
        bool        operator != (const Vector2Int& v) const { return !(*this == v); }

        String toString() const { return "(" + TS(x) + "," + TS(y) + ")"; }
    };

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

        bool        operator == (const Vector3F& v) const { return x == v.x && y == v.y && z == v.z; }
        bool        operator != (const Vector3F& v) const { return !(*this == v); }

        F32         operator[] (I32 index) const    { return reinterpret_cast<const F32*>(&x)[index]; }
        F32&        operator[] (I32 index)          { return reinterpret_cast<F32*>(&x)[index]; }

        F32         magnitude() const;
        Vector3F    normalized() const;
        Vector3F    cross(const Vector3F& v) const;
        F32         maxValue() const;
        F32         minValue() const;
        F32         distance(const Vector3F& v) const;
        Vector3F    minVec(const Vector3F& v) const;
        Vector3F    maxVec(const Vector3F& v) const;

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

        bool        operator == (const Vector4F& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
        bool        operator != (const Vector4F& v) const { return !(*this == v); }

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
        Vector3F    toEulerAngles() const;

        // Returns the axis angle representation with the x,y,z as the axis and w as the angle in DEGREES
        Vector4F    toAxisAngle() const;

        Vector3F getForward()   const { return *this * Vector3F::FORWARD; }
        Vector3F getBack()      const { return *this * Vector3F::BACK; }
        Vector3F getLeft()      const { return *this * Vector3F::LEFT; }
        Vector3F getRight()     const { return *this * Vector3F::RIGHT; }
        Vector3F getUp()        const { return *this * Vector3F::UP; }
        Vector3F getDown()      const { return *this * Vector3F::DOWN; }

        static Quaternion LookRotation(const Vector3F& forward, const Vector3F& up);
        static Quaternion FromEulerAngles(F32 pitchDegrees, F32 yawDegrees, F32 rollDegrees);
        static Quaternion FromEulerAngles(const Vector3F& eulerAnglesInDegrees);
        static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, F32 t);

        String toString() const { return "(" + TS(x) + "," + TS(y) + "," + TS(z) + "," + TS(w) + ")"; }

        // Static members
        static const Quaternion IDENTITY;
    };

    //----------------------------------------------------------------------
    using Vec2      = Vector2F;
    using Vec2Int   = Vector2Int;
    using Vec3      = Vector3F;
    using Vec4      = Vector4F;
    using Quat      = Quaternion;

}





#else
    static_assert("DX Math-Wrapper not supported on this plattform!");
#endif