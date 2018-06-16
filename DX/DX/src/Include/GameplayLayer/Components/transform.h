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

        const ArrayList<Transform*>&    getChildren()   const { return m_pChildren; }
        const Transform*                getParent()     const { return m_pParent; }
        Math::Vec3                      getWorldPosition() const;
        Math::Vec3                      getWorldScale() const;
        Math::Quat                      getWorldRotation() const;
        void                            getWorldTransform(Math::Vec3* pos, Math::Vec3* scale, Math::Quat* quat);

        //----------------------------------------------------------------------
        // Rotates this transform to look at the given target. 
        // (P.S. This might be incorrect if the target has the opposite direction from the world up vector i.e. looking straight down)
        //----------------------------------------------------------------------
        void lookAt(const Math::Vec3& target);

        //----------------------------------------------------------------------
        // Set the parent for this transform component.
        // @Params:
        //  "t": This transform becomes the new parent transform.
        //  "keepWorldTransform": If true, this component will keep the current world transform.
        //----------------------------------------------------------------------
        void setParent(Transform* t, bool keepWorldTransform = true);

        //----------------------------------------------------------------------
        // Add a child to this transform.
        // @Params:
        //  "t": This transform becomes the new child.
        //  "keepWorldTransform": If true, the child will keep his current world transform.
        //----------------------------------------------------------------------
        void addChild(Transform* t, bool keepWorldTransform = true);

        //----------------------------------------------------------------------
        // Returns the final composited world transformation matrix.
        //----------------------------------------------------------------------
        DirectX::XMMATRIX getWorldMatrix() const;

        //----------------------------------------------------------------------
        // Returns the final composited world trasnformation matrix lerped between this
        // and the last call. Therefore this function should be called once per render.
        // !! THIS IS ONLY A TEST TO TRY TO FIX THE STUTTERING ISSUE !!
        //----------------------------------------------------------------------
        DirectX::XMMATRIX getWorldMatrix(F32 lerp);

    private:
        Transform*            m_pParent = nullptr;
        ArrayList<Transform*> m_pChildren;
        DirectX::XMMATRIX     m_lastWorldMatrix = DirectX::XMMatrixScaling(0, 0, 0);

        inline void _RemoveFromParent();
        inline DirectX::XMMATRIX _GetLocalTransformationMatrix() const;

        NULL_COPY_AND_ASSIGN(Transform)
    };

}