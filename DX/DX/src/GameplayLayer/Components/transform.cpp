#include "transform.h"
/**********************************************************************
    class: Transform (transform.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

namespace Components {

    //----------------------------------------------------------------------
    void Transform::lookAt( const Math::Vec3& target )
    {
        Math::Vec3 forward = (target - position).normalized();
        rotation = Math::Quat::LookRotation(forward, Math::Vec3::UP);
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX Transform::getTransformationMatrix()
    {
        DirectX::XMVECTOR s = DirectX::XMLoadFloat3( &scale );
        DirectX::XMVECTOR r = DirectX::XMLoadFloat4( &rotation );
        DirectX::XMVECTOR p = DirectX::XMLoadFloat3( &position );

        return DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );
    }

    //----------------------------------------------------------------------
    void Transform::setParent(const Transform& t)
    {
        ASSERT(false);
    }

    //----------------------------------------------------------------------
    void Transform::addChild(const Transform& t)
    {
        ASSERT(false);
    }


}