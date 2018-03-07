#include "dxmath_wrapper.h"
/**********************************************************************
    class: Several (dxmath_wrapper.cpp)
    
    author: S. Hau
    date: March 3, 2018

**********************************************************************/


#ifdef _WIN32

namespace Math {

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
        : DirectX::XMFLOAT3( value, value, value )
    {
    }

    //----------------------------------------------------------------------
    Vector3F::Vector3F( F32 x, F32 y, F32 z )
        : DirectX::XMFLOAT3( x, y, z )
    {
    }

    //----------------------------------------------------------------------
    F32 Vector3F::magnitude() const
    {
        return sqrt( x * x + y * y + z * z );
    }

    //----------------------------------------------------------------------
    Vector3F Vector3F::normalized() const
    {
        F32 length = this->magnitude();
        if ( length == 0.0f )
            return Vector3F(0);
        return *this / magnitude();
    }

    //----------------------------------------------------------------------
    Vector3F Vector3F::cross( const Vector3F& v ) const
    {
        return Vector3F( y * v.z - v.y * z, v.x * z - x * v.z, x * v.y - v.x * y );
    }

    //**********************************************************************
    // Vector4F
    //**********************************************************************

    //----------------------------------------------------------------------
    Vector4F::Vector4F( F32 value )
        : DirectX::XMFLOAT4( value, value, value, value )
    {
    }

    //----------------------------------------------------------------------
    Vector4F::Vector4F( F32 x, F32 y, F32 z, F32 w )
        : DirectX::XMFLOAT4( x, y, z, w )
    {
    }

    //**********************************************************************
    // Quaternion
    //**********************************************************************

    const Quaternion Quaternion::IDENTITY( 0, 0, 0 ,1 );

    //----------------------------------------------------------------------
    Quaternion::Quaternion( F32 x, F32 y, F32 z, F32 w )
        : DirectX::XMFLOAT4( x, y, z, w )
    {
    }

    //----------------------------------------------------------------------
    Quaternion::Quaternion( const Vector3F& axis, F32 angleInDegrees )
    {
        DirectX::XMVECTOR ax = DirectX::XMLoadFloat3( &axis );
        DirectX::XMVECTOR result = DirectX::XMQuaternionRotationAxis( ax, DirectX::XMConvertToRadians( angleInDegrees ) );
        DirectX::XMStoreFloat4( this, result );
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
        DirectX::XMVECTOR thisQuaternion = DirectX::XMLoadFloat4( this );
        DirectX::XMVECTOR otherVector = DirectX::XMLoadFloat3( &v );
        DirectX::XMVECTOR result = DirectX::XMVector3Rotate( otherVector, thisQuaternion );

        Vector3F resultVector;
        DirectX::XMStoreFloat3( &resultVector, result );

        return resultVector;
    }

    //----------------------------------------------------------------------
    Quaternion& Quaternion::operator *= (const Quaternion& q)
    {
        DirectX::XMVECTOR thisQuaternion = DirectX::XMLoadFloat4( this );
        DirectX::XMVECTOR otherQuaternion = DirectX::XMLoadFloat4( &q );
        DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply( thisQuaternion, otherQuaternion );

        DirectX::XMStoreFloat4( this, result );

        return *this;
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

}


#endif