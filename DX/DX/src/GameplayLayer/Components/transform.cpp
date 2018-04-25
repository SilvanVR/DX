#include "transform.h"
/**********************************************************************
    class: Transform (transform.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#define LERP_TRANSFORM 0

namespace Components {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Transform::getWorldTransform( Math::Vec3* pos, Math::Vec3* scale, Math::Quat* quat )
    {
        DirectX::XMVECTOR s, r, p;
        DirectX::XMMatrixDecompose( &s, &r, &p, getTransformationMatrix() );

        DirectX::XMStoreFloat3( pos, p );
        DirectX::XMStoreFloat3( scale, s );
        DirectX::XMStoreFloat4( quat, r );
    }

    //----------------------------------------------------------------------
    Math::Vec3 Transform::getWorldPosition() const
    {
        DirectX::XMVECTOR s, r, p;
        DirectX::XMMatrixDecompose( &s, &r, &p, getTransformationMatrix() );

        Math::Vec3 worldPos;
        DirectX::XMStoreFloat3( &worldPos, p );
        return worldPos;
    }

    //----------------------------------------------------------------------
    Math::Vec3 Transform::getWorldScale() const
    {
        DirectX::XMVECTOR s, r, p;
        DirectX::XMMatrixDecompose( &s, &r, &p, getTransformationMatrix() );

        Math::Vec3 worldScale;
        DirectX::XMStoreFloat3( &worldScale, s );
        return worldScale;
    }

    //----------------------------------------------------------------------
    Math::Quat Transform::getWorldRotation() const
    {
        DirectX::XMVECTOR s, r, p;
        DirectX::XMMatrixDecompose( &s, &r, &p, getTransformationMatrix() );

        Math::Quat worldRotation;
        DirectX::XMStoreFloat4( &worldRotation, r );
        return worldRotation;
    }

    //----------------------------------------------------------------------
    void Transform::lookAt( const Math::Vec3& target )
    {
        Math::Vec3 forward = (target - position).normalized();
        rotation = Math::Quat::LookRotation( forward, Math::Vec3::UP );
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX Transform::getTransformationMatrix() const
    {
        auto transformationMatrix = _GetLocalTransformationMatrix();
        if (m_pParent)
        {
            auto parentMatrix = m_pParent->getTransformationMatrix();
            return DirectX::XMMatrixMultiply( transformationMatrix, parentMatrix );
        }
        return transformationMatrix;
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX Transform::getTransformationMatrix( F32 lerp )
    {
        auto modelMatrix = getTransformationMatrix();
#if LERP_TRANSFORM
        DirectX::XMVECTOR sCur, rCur, pCur, sPrev, rPrev, pPrev;
        DirectX::XMMatrixDecompose( &sCur, &rCur, &pCur, modelMatrix );
        DirectX::XMMatrixDecompose( &sPrev, &rPrev, &pPrev, m_lastModelMatrix );

        auto s = DirectX::XMVectorLerp( sPrev, sCur, lerp );
        auto p = DirectX::XMVectorLerp( pPrev, pCur, lerp );
        auto r = DirectX::XMQuaternionSlerp( rPrev, rCur, lerp );

        modelMatrix = DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );
        m_lastModelMatrix = modelMatrix;
#endif
        return modelMatrix;
    }

    //----------------------------------------------------------------------
    void Transform::setParent( Transform* parent, bool keepWorldTransform )
    {
        if (keepWorldTransform)
        {
            auto matrix = getTransformationMatrix();

            // If the new parent is valid the current world transform must be transformed into this new parents space in order to get the new world transform
            // This is done by multiplying the current transformation matrix with the new parents inverse.
            if (parent)
                matrix = DirectX::XMMatrixMultiply( matrix, DirectX::XMMatrixInverse( NULL, parent->getTransformationMatrix() ) );

            DirectX::XMVECTOR s, r, p;
            DirectX::XMMatrixDecompose( &s, &r, &p, matrix );

            DirectX::XMStoreFloat3( &position, p );
            DirectX::XMStoreFloat3( &scale, s );
            DirectX::XMStoreFloat4( &rotation, r );
        }

        // Remove from current parent 
        if (m_pParent)
            this->_RemoveFromParent();

        m_pParent = parent;

        // Add to new parent
        if (m_pParent)
            m_pParent->m_pChildren.emplace_back( this );
    }

    //----------------------------------------------------------------------
    void Transform::addChild( Transform* child, bool keepWorldTransform )
    {
        ASSERT( child != nullptr );
        child->setParent( this, keepWorldTransform );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Transform::_RemoveFromParent()
    {
        ASSERT( m_pParent );
        m_pParent->m_pChildren.erase( std::remove( m_pParent->m_pChildren.begin(), m_pParent->m_pChildren.end(), this ) );
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX Transform::_GetLocalTransformationMatrix() const
    {
        DirectX::XMVECTOR s = DirectX::XMLoadFloat3( &scale );
        DirectX::XMVECTOR r = DirectX::XMLoadFloat4( &rotation );
        DirectX::XMVECTOR p = DirectX::XMLoadFloat3( &position );

        return DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );
    }

}