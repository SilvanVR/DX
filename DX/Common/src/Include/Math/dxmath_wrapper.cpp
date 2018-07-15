#include "dxmath_wrapper.h"
/**********************************************************************
    class: Several (dxmath_wrapper.cpp)
    
    author: S. Hau
    date: March 3, 2018

**********************************************************************/

#define _USE_MATH_DEFINES
#include <math.h> // M_PI

#ifdef _WIN32

using namespace DirectX;

namespace Math {

    //**********************************************************************
    // Vector2F
    //**********************************************************************

    const Vector2F Vector2F::UP     ( 0,  1 );
    const Vector2F Vector2F::DOWN   ( 0, -1 );
    const Vector2F Vector2F::LEFT   (-1,  0 );
    const Vector2F Vector2F::RIGHT  ( 1,  0 );
    const Vector2F Vector2F::ONE    ( 1,  1 );
    const Vector2F Vector2F::ZERO   ( 0,  0 );

    //----------------------------------------------------------------------
    Vector2F::Vector2F( F32 value )
        : XMFLOAT2( value, value )
    {}

    //----------------------------------------------------------------------
    Vector2F::Vector2F( F32 x, F32 y )
        : XMFLOAT2( x, y )
    {}

    //----------------------------------------------------------------------
    F32 Vector2F::magnitude() const
    {
        return sqrt( x * x + y * y );
    }

    //----------------------------------------------------------------------
    Vector2F Vector2F::normalized() const
    {
        F32 length = this->magnitude();
        if ( length == 0.0f )
            return Vector2F(0);
        return *this / magnitude();
    }

    //**********************************************************************
    // Vector2Int
    //**********************************************************************

    //----------------------------------------------------------------------
    Vector2Int::Vector2Int( I32 value )
        : XMINT2( value, value )
    {}

    //----------------------------------------------------------------------
    Vector2Int::Vector2Int( I32 x, I32 y )
        : XMINT2( x, y )
    {}

    //**********************************************************************
    // Vector3F
    //**********************************************************************

    const Vector3F Vector3F::UP     ( 0,  1,  0);
    const Vector3F Vector3F::DOWN   ( 0, -1,  0);
    const Vector3F Vector3F::LEFT   (-1,  0,  0);
    const Vector3F Vector3F::RIGHT  ( 1,  0,  0);
    const Vector3F Vector3F::FORWARD( 0,  0,  1);
    const Vector3F Vector3F::BACK   ( 0,  0, -1);
    const Vector3F Vector3F::ONE    ( 1,  1,  1);
    const Vector3F Vector3F::ZERO   ( 0,  0,  0);

    //----------------------------------------------------------------------
    Vector3F::Vector3F( F32 value )
        : XMFLOAT3( value, value, value )
    {}

    //----------------------------------------------------------------------
    Vector3F::Vector3F( F32 x, F32 y, F32 z )
        : XMFLOAT3( x, y, z )
    {}

    //----------------------------------------------------------------------
    F32 Vector3F::magnitude() const
    {
        return sqrt( x * x + y * y + z * z );
    }

    //----------------------------------------------------------------------
    F32 Vector3F::magnitudeSqrt() const
    {
        return x * x + y * y + z * z;
    }

    //----------------------------------------------------------------------
    Vector3F Vector3F::normalized() const
    {
        F32 length = this->magnitude();
        if ( length == 0.0f )
            return Vector3F(0);
        return *this / length;
    }

    //----------------------------------------------------------------------
    void Vector3F::normalize()
    {
        F32 length = this->magnitude();
        if ( length == 0.0f )
            return;
        x /= length; 
        y /= length; 
        z /= length;
    }


    //----------------------------------------------------------------------
    Vector3F Vector3F::cross( const Vector3F& v ) const
    {
        return Vector3F( y * v.z - v.y * z, v.x * z - x * v.z, x * v.y - v.x * y );
    }

    //----------------------------------------------------------------------
    F32 Vector3F::maxValue() const
    {
        return x > y ? (x > z ? x : z) : (y > z ? y : z);
    }

    //----------------------------------------------------------------------
    F32 Vector3F::minValue() const
    {
        return x < y ? (x < z ? x : z) : (y < z ? y : z);
    }

    //----------------------------------------------------------------------
    F32 Vector3F::distance( const Vector3F& v ) const
    {
        return (*this - v).magnitude();
    }

    //----------------------------------------------------------------------
    F32 Vector3F::distanceSqrt( const Vector3F& v ) const
    {
        return (*this - v).magnitudeSqrt();
    }

    //----------------------------------------------------------------------
    Vector3F Vector3F::minVec( const Vector3F& v ) const
    {
        F32 x, y, z;
        this->x < v.x ? x = this->x : x = v.x;
        this->y < v.y ? y = this->y : y = v.y;
        this->z < v.z ? z = this->z : z = v.z;
        return Vector3F( x, y, z );
    }
    
    //----------------------------------------------------------------------
    Vector3F Vector3F::maxVec( const Vector3F& v ) const
    {
        F32 x, y, z;
        this->x > v.x ? x = this->x : x = v.x;
        this->y > v.y ? y = this->y : y = v.y;
        this->z > v.z ? z = this->z : z = v.z;
        return Vector3F( x, y, z );
    }

    //**********************************************************************
    // Vector4F
    //**********************************************************************

    //----------------------------------------------------------------------
    Vector4F::Vector4F( F32 value )
        : XMFLOAT4( value, value, value, value )
    {
    }

    //----------------------------------------------------------------------
    Vector4F::Vector4F( F32 x, F32 y, F32 z, F32 w )
        : XMFLOAT4( x, y, z, w )
    {
    }

    //----------------------------------------------------------------------
    Vector4F::Vector4F( const std::array<F32, 4>& arr )
        : XMFLOAT4( arr[0], arr[1], arr[2], arr[3])
    {
    }

    //**********************************************************************
    // Quaternion
    //**********************************************************************

    const Quaternion Quaternion::IDENTITY( 0, 0, 0 ,1 );

    //----------------------------------------------------------------------
    Quaternion::Quaternion( F32 x, F32 y, F32 z, F32 w )
        : XMFLOAT4( x, y, z, w )
    {
    }

    //----------------------------------------------------------------------
    Quaternion::Quaternion( const Vector3F& axis, F32 angleInDegrees )
    {
        XMVECTOR ax = XMLoadFloat3( &axis );
        XMVECTOR result = XMQuaternionRotationAxis( ax, XMConvertToRadians( angleInDegrees ) );
        XMStoreFloat4( this, result );
    }

    //----------------------------------------------------------------------
    Quaternion Quaternion::operator * ( const Quaternion& q ) const
    {
        Quaternion resultQuaternion = *this;
        resultQuaternion *= q;

        return resultQuaternion;
    }

    //----------------------------------------------------------------------
    Vector3F Quaternion::operator * ( const Vector3F& v ) const
    {
        XMVECTOR thisQuaternion = XMLoadFloat4( this );
        XMVECTOR otherVector = XMLoadFloat3( &v );
        XMVECTOR result = XMVector3Rotate( otherVector, thisQuaternion );

        Vector3F resultVector;
        XMStoreFloat3( &resultVector, result );

        return resultVector;
    }

    //----------------------------------------------------------------------
    Quaternion& Quaternion::operator *= (const Quaternion& q)
    {
        XMVECTOR thisQuaternion = XMLoadFloat4( this );
        XMVECTOR otherQuaternion = XMLoadFloat4( &q );
        XMVECTOR result = XMQuaternionMultiply( thisQuaternion, otherQuaternion );

        XMStoreFloat4( this, result );

        return *this;
    }

    //----------------------------------------------------------------------
    F32 Quaternion::magnitude() const
    {
        return sqrt( x * x + y * y + z * z + w * w );
    }

    //----------------------------------------------------------------------
    Quaternion Quaternion::normalized() const
    {
        F32 length = this->magnitude();
        if ( length == 0.0f )
            return Quaternion(0);
        return Quaternion( x / length, y / length, z / length, w / length );
    }

    //----------------------------------------------------------------------
    Vector4F Quaternion::toAxisAngle() const
    {
        XMVECTOR axis;
        F32 radians;

        XMVECTOR quat = XMLoadFloat4( this );
        XMQuaternionToAxisAngle( &axis, &radians, quat );

        Vector4F result;
        XMStoreFloat4( &result, quat );
        result.w = XMConvertToDegrees( radians );

        return result;
    }

    //----------------------------------------------------------------------
    Vector3F Quaternion::toEulerAngles() const
    {
        Math::Vec3 eulerAngles;

        F32 test = x*y + z*w;
        if (test > 0.499f) { // singularity at north pole
            eulerAngles.x = 0;
            eulerAngles.y = 2 * atan2f( x, w );
            eulerAngles.z = F32( M_PI ) / 2;
        }
        else if (test < -0.499) { // singularity at south pole
            eulerAngles.x = 0;
            eulerAngles.y = -2 * atan2f( x, w );
            eulerAngles.z = - F32( M_PI ) / 2;
        }
        else {
            F32 sqz = z*z;
            eulerAngles.x = atan2f( 2 * x*w - 2 * y*z, 1 - 2 * x*x - 2 * sqz );
            eulerAngles.y = atan2f( 2 * y*w - 2 * x*z, 1 - 2 * y*y - 2 * sqz );
            eulerAngles.z = asinf( 2 * test );
        }

        eulerAngles.x = XMConvertToDegrees( eulerAngles.x );
        eulerAngles.y = XMConvertToDegrees( eulerAngles.y );
        eulerAngles.z = XMConvertToDegrees( eulerAngles.z );

        return eulerAngles;
    }

    //----------------------------------------------------------------------
    Quaternion Quaternion::conjugate() const
    {
        auto quat = XMLoadFloat4( this );
        auto conjugate = XMQuaternionConjugate( quat );

        Quaternion result;
        XMStoreFloat4( &result, conjugate );
        return result;
    }

    //----------------------------------------------------------------------
    Quaternion Quaternion::LookRotation( const Vector3F& forward, const Vector3F& up )
    {
        // Code from https://pastebin.com/ubATCxJY
        Vector3F vector = forward.normalized();
        Vector3F vector2 = up.cross( vector ).normalized();
        Vector3F vector3 = vector.cross( vector2 );
        F32 m00 = vector2.x;
        F32 m01 = vector2.y;
        F32 m02 = vector2.z;
        F32 m10 = vector3.x;
        F32 m11 = vector3.y;
        F32 m12 = vector3.z;
        F32 m20 = vector.x;
        F32 m21 = vector.y;
        F32 m22 = vector.z;

        F32 num8 = (m00 + m11) + m22;
        Quaternion quaternion;
        if (num8 > 0.0)
        {
            F32 num = sqrtf(num8 + 1.0f);
            quaternion.w = (num * 0.5f);
            num = 0.5f / num;
            quaternion.x = (m12 - m21) * num;
            quaternion.y = (m20 - m02) * num;
            quaternion.z = (m01 - m10) * num;
            return quaternion;
        }
        if ((m00 >= m11) && (m00 >= m22))
        {
            F32 num7 = sqrtf(((1.0f + m00) - m11) - m22);
            F32 num4 = 0.5f / num7;
            quaternion.x = (0.5f * num7);
            quaternion.y = (m01 + m10) * num4;
            quaternion.z = (m02 + m20) * num4;
            quaternion.w = (m12 - m21) * num4;
            return quaternion;
        }
        if (m11 > m22)
        {
            F32 num6 = sqrtf(((1.0f + m11) - m00) - m22);
            F32 num3 = 0.5f / num6;
            quaternion.x = (m10 + m01) * num3;
            quaternion.y = (0.5f * num6);
            quaternion.z = (m21 + m12) * num3;
            quaternion.w = (m20 - m02) * num3;
            return quaternion;
        }
        F32 num5 = sqrtf(((1.0f + m22) - m00) - m11);
        F32 num2 = 0.5f / num5;
        quaternion.x = (m20 + m02) * num2;
        quaternion.y = (m21 + m12) * num2;
        quaternion.z = (0.5f * num5);
        quaternion.w = (m01 - m10) * num2;

        return quaternion;
    }

    ////----------------------------------------------------------------------
    // Somehow this functions produces weird results. Transfering it back to euler angles produces even weirder results.
    //Quaternion Quaternion::FromEulerAngles( F32 pitch, F32 yaw, F32 roll )
    //{
    //    Quaternion quaternion;
    //    auto result = XMQuaternionRotationRollPitchYaw( XMConvertToRadians( pitch ), 
    //                                                    XMConvertToRadians( yaw ), 
    //                                                    XMConvertToRadians( roll ) );
    //    XMStoreFloat4( &quaternion, result );
    //    return quaternion;
    //}

    //----------------------------------------------------------------------
    Quaternion Quaternion::FromEulerAngles( F32 pitch, F32 yaw, F32 roll )
    {
        Quaternion quaternion;
        F32 xRad = XMConvertToRadians( pitch );
        F32 yRad = XMConvertToRadians( yaw );
        F32 zRad = XMConvertToRadians( roll );
        
        F32 c1 = cos( yRad / 2 );
        F32 s1 = sin( yRad / 2 );
        F32 c2 = cos( zRad / 2 );
        F32 s2 = sin( zRad / 2 );
        F32 c3 = cos( xRad / 2 );
        F32 s3 = sin( xRad / 2 );
        F32 c1c2 = c1 * c2;
        F32 s1s2 = s1 * s2;

        return Quaternion( c1c2*s3 + s1s2 * c3, s1*c2*c3 + c1 * s2*s3, c1*s2*c3 - s1 * c2*s3, c1c2*c3 - s1s2 * s3 ).normalized();
    }

    //----------------------------------------------------------------------
    Quaternion Quaternion::FromEulerAngles( const Vector3F& eulerAngles )
    {
        return FromEulerAngles( eulerAngles.x, eulerAngles.y, eulerAngles.z );
    }

    //----------------------------------------------------------------------
    Quaternion Quaternion::Slerp( const Quaternion& q1, const Quaternion& q2, F32 t )
    {
        XMVECTOR quat1  = XMLoadFloat4( &q1 );
        XMVECTOR quat2  = XMLoadFloat4( &q2 );
        XMVECTOR result = XMQuaternionSlerp( quat1, quat2, t );

        Quaternion q;
        XMStoreFloat4( &q, result );
        return q;
    }

}


#endif