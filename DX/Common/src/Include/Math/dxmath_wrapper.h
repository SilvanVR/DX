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
        Vector3F(float value = 0);
        Vector3F(float x, float y, float z);

        Vector3F    operator +      (const Vector3F& v) const { return Vector3F( x + v.x, y + v.y, z + v.z ); }
        Vector3F    operator -      (const Vector3F& v) const { return Vector3F( x - v.x, y - v.y, z - v.z ); }
        Vector3F&   operator +=     (const Vector3F& v) { x += v.x, y += v.y; z += v.z; return *this; }
        Vector3F&   operator -=     (const Vector3F& v) { x -= v.x, y -= v.y; z -= v.z; return *this; }

        // Static members
        static const Vector3F UP;            //same as ( 0, 1, 0 )
        static const Vector3F DOWN;          //same as ( 0,-1, 0 )
        static const Vector3F LEFT;          //same as ( 1, 0, 0 )
        static const Vector3F RIGHT;         //same as (-1, 0, 0 )
        static const Vector3F FORWARD;       //same as ( 0, 0, 1 )
        static const Vector3F BACK;          //same as ( 0, 0,-1 )
        static const Vector3F ONE;           //same as ( 1, 1, 1 )
        static const Vector3F ZERO;          //same as ( 0, 0, 0 )
    };

    //**********************************************************************
    class Vector4F : public DirectX::XMFLOAT4
    {
    public:
        Vector4F(float value = 0);
        Vector4F(float x, float y, float z, float w);

        Vector4F    operator +      (const Vector4F& v) const { return Vector4F( x + v.x, y + v.y, z + v.z, w + v.w ); }
        Vector4F    operator -      (const Vector4F& v) const { return Vector4F( x - v.x, y - v.y, z - v.z, w - v.w ); }
        Vector4F&   operator +=     (const Vector4F& v) { x += v.x, y += v.y; z += v.z; w += v.w; return *this; }
        Vector4F&   operator -=     (const Vector4F& v) { x -= v.x, y -= v.y; z -= v.z; w -= v.w; return *this; }
    };

    //**********************************************************************
    class Quaternion : public DirectX::XMFLOAT4
    {
    public:
        Quaternion(float x = 0, float y = 0, float z = 0, float w = 1);

        Quaternion    operator +      (const Quaternion& v) const { return Quaternion(x + v.x, y + v.y, z + v.z, w + v.w); }
        Quaternion    operator -      (const Quaternion& v) const { return Quaternion(x - v.x, y - v.y, z - v.z, w - v.w); }
        Quaternion&   operator +=     (const Quaternion& v) { x += v.x, y += v.y; z += v.z; w += v.w; return *this; }
        Quaternion&   operator -=     (const Quaternion& v) { x -= v.x, y -= v.y; z -= v.z; w -= v.w; return *this; }

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