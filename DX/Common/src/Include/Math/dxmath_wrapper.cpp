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
    const Vector3F Vector3F::LEFT   ( 1,  0,  0);
    const Vector3F Vector3F::RIGHT  (-1,  0,  0);
    const Vector3F Vector3F::FORWARD( 0,  0,  1);
    const Vector3F Vector3F::BACK   ( 0,  0, -1);
    const Vector3F Vector3F::ONE    ( 1,  1,  1);
    const Vector3F Vector3F::ZERO   ( 0,  0,  0);

    //----------------------------------------------------------------------
    Vector3F::Vector3F( float value )
        : DirectX::XMFLOAT3( value, value, value )
    {
    }

    //----------------------------------------------------------------------
    Vector3F::Vector3F( float x, float y, float z )
        : DirectX::XMFLOAT3( x, y, z )
    {        
    }

    //**********************************************************************
    // Vector4F
    //**********************************************************************

    //----------------------------------------------------------------------
    Vector4F::Vector4F( float value )
        : DirectX::XMFLOAT4( value, value, value, value )
    {
    }

    //----------------------------------------------------------------------
    Vector4F::Vector4F( float x, float y, float z, float w )
        : DirectX::XMFLOAT4( x, y, z, w )
    {
    }

    //**********************************************************************
    // Quaternion
    //**********************************************************************

    const Quaternion Quaternion::IDENTITY( 0, 0, 0 ,1 );

    //----------------------------------------------------------------------
    Quaternion::Quaternion( float x, float y, float z, float w )
        : DirectX::XMFLOAT4( x, y, z, w )
    {
    }

}


#endif