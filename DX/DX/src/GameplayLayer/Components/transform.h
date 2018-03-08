#pragma once
/**********************************************************************
    class: Transform (transform.h)

    author: S. Hau
    date: December 17, 2017

**********************************************************************/

#include "i_component.h"

namespace Components {

    //**********************************************************************
    class Transform : public IComponent
    {
    public:
        Transform() {}

        Math::Vec3 position = Math::Vec3( 0.0f, 0.0f, 0.0f );
        Math::Vec3 scale    = Math::Vec3( 1.0f, 1.0f, 1.0f );
        Math::Quat rotation = Math::Quat( 0.0f, 0.0f, 0.0f, 1.0f );

        //----------------------------------------------------------------------
        void lookAt(const Math::Vec3& target);

        //----------------------------------------------------------------------
        void setParent(const Transform& t);
        void addChild(const Transform& t);


        //----------------------------------------------------------------------
        DirectX::XMMATRIX getTransformationMatrix();

    private:
        Transform*              m_pParent = nullptr;
        ArrayList<Transform>    m_pChildren;

        //----------------------------------------------------------------------
        Transform(const Transform& other)               = delete;
        Transform& operator = (const Transform& other)  = delete;
        Transform(Transform&& other)                    = delete;
        Transform& operator = (Transform&& other)       = delete;
    };

}