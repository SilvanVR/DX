#include "skinned_mesh_renderer.h"
/**********************************************************************
    class: SkinnedMeshRenderer

    author: S. Hau
    date: October 29, 2019
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "Common/finite_range.hpp"
#include "Math/math_utils.h"
#include "../transform.h"
#include "camera.h"

namespace Components {

    static constexpr StringID SID_BONE_TRANSFORMS("u_boneTransforms");

    //----------------------------------------------------------------------
    SkinnedMeshRenderer::SkinnedMeshRenderer( const MeshPtr& mesh, const Animation::Skeleton& skeleton, 
                                             const Animation::AnimationClip& animation, const MaterialPtr& material )
        : MeshRenderer( mesh, material ), m_skeleton( skeleton )
    {
        const auto& ubo = material->getShader()->getVSUniformShaderBuffer();
        if (auto member = ubo->getMember(SID_BONE_TRANSFORMS))
        {
            m_matrixPalette.resize( member->getArraySize(), DirectX::XMMatrixIdentity() );
            m_jointWorldMatrices.resize( member->getArraySize(), DirectX::XMMatrixIdentity() );
        }
        else
            LOG_WARN( "SkinnedMeshRenderer(): Shader from given material has no uniform buffer for skinning matrices." );

        playAnimation( animation );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void SkinnedMeshRenderer::tick( Time::Seconds delta )
    {
        if (m_matrixPalette.empty())
            return;

        bool isRunning = m_clock.tick( delta );
        if (isRunning)
        {
            Time::Seconds clockTime = m_clock.getTime();

            // Lerp intermediate poses from animation
            for (U32 joint = 0; joint < m_skeleton.joints.size(); joint++)
            {
                auto p = _GetInterpolatedTranslation( joint, clockTime );
                auto r = _GetInterpolatedRotation( joint, clockTime );
                auto s = _GetInterpolatedScale( joint, clockTime );
                m_jointWorldMatrices[joint] = DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );
            }

            // Calculate global pose matrices
            for (U32 i = 0; i < m_skeleton.joints.size(); i++)
            {
                auto& joint = m_skeleton.joints[i];
                if (joint.parentIndex >= 0)
                    m_jointWorldMatrices[i] = m_jointWorldMatrices[i] * m_jointWorldMatrices[joint.parentIndex];
            }

            // Calculate matrix palette and update shader ubo
            // (This is just proof of concept and works only for one animation)
            for (U32 i = 0; i < m_skeleton.joints.size(); i++)
            {
                auto& joint = m_skeleton.joints[i];
                m_matrixPalette[i] = joint.invBindPose * m_jointWorldMatrices[i];
            }

            auto& shader = getMaterial()->getShader();
            shader->setData( SID_BONE_TRANSFORMS, m_matrixPalette.data() );
        }
    }

    //----------------------------------------------------------------------
    void SkinnedMeshRenderer::playAnimation( const Animation::AnimationClip& animation )
    {
        if (animation.jointSamples.size() != m_skeleton.joints.size())
        {
            LOG_WARN( "SkinnedMeshRenderer(): Skeleton and Animation have a different amount of joints, which is not allowed." );
            return;
        }
        m_animation = animation;
        m_clock.setDuration( animation.duration );
        m_clock.setTime( 0_ms );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    DirectX::XMVECTOR SkinnedMeshRenderer::_GetInterpolatedTranslation( U32 joint, Time::Seconds clockTime )
    {
        auto& transKeys = m_animation.jointSamples[joint].translationKeys;

        if (transKeys.size() == 1)
            return DirectX::XMLoadFloat3( &transKeys.front().translation );

        Common::FiniteRange<I32> begin( 0, 0, (I32)transKeys.size() );
        Common::FiniteRange<I32> end( 1, 0, (I32)transKeys.size() );
        while (clockTime > transKeys[end.value()].time)
        {
            begin += 1;
            end += 1;
        }
        F32 lerp = (F32)((clockTime - transKeys[begin.value()].time).value / 
                         (transKeys[ end.value() ].time - transKeys[ begin.value() ].time).value);
        ASSERT( lerp >= 0.0f && lerp <= 1.0f );
        auto pBegin = DirectX::XMLoadFloat3( &transKeys[ begin.value() ].translation );
        auto pEnd = DirectX::XMLoadFloat3( &transKeys[ end.value() ].translation );

        return DirectX::XMVectorLerp( pBegin, pEnd, lerp );
    }

    //----------------------------------------------------------------------
    DirectX::XMVECTOR SkinnedMeshRenderer::_GetInterpolatedRotation( U32 joint, Time::Seconds clockTime )
    {
        auto& rotKeys = m_animation.jointSamples[joint].rotationKeys;

        if (rotKeys.size() == 1)
            return DirectX::XMLoadFloat4( &rotKeys.front().rotation );

        Common::FiniteRange<I32> begin( 0, 0, (I32)rotKeys.size() );
        Common::FiniteRange<I32> end( 1, 0, (I32)rotKeys.size() );
        while (clockTime > rotKeys[ end.value() ].time)
        {
            begin += 1;
            end += 1;
        }
        F32 lerp = (F32)( (clockTime - rotKeys[begin.value()].time).value / 
                          (rotKeys[ end.value() ].time - rotKeys[ begin.value() ].time).value);
        auto rBegin = DirectX::XMLoadFloat4( &rotKeys[begin.value()].rotation );
        auto rEnd = DirectX::XMLoadFloat4( &rotKeys[end.value()].rotation );

        return DirectX::XMQuaternionSlerp( rBegin, rEnd, lerp );
    }

    //----------------------------------------------------------------------
    DirectX::XMVECTOR SkinnedMeshRenderer::_GetInterpolatedScale( U32 joint, Time::Seconds clockTime )
    {
        auto& scaleKeys = m_animation.jointSamples[joint].scalingKeys;

        if (scaleKeys.size() == 1)
            return DirectX::XMLoadFloat3( &scaleKeys.front().scale );

        Common::FiniteRange<I32> begin( 0, 0, (I32)scaleKeys.size() );
        Common::FiniteRange<I32> end( 1, 0, (I32)scaleKeys.size() );
        while (clockTime > scaleKeys[ end.value() ].time)
        {
            begin += 1;
            end += 1;
        }
        F32 lerp = (F32)( (clockTime - scaleKeys[begin.value()].time).value /
                          (scaleKeys[ end.value() ].time - scaleKeys[ begin.value() ].time).value);
        auto sBegin = DirectX::XMLoadFloat3( &scaleKeys[begin.value()].scale );
        auto sEnd = DirectX::XMLoadFloat3( &scaleKeys[end.value()].scale );

        return DirectX::XMVectorLerp( sBegin, sEnd, lerp );
    }
}