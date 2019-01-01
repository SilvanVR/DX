#pragma once
/**********************************************************************
    class: SkinnedSkinnedMeshRenderer

    author: S. Hau
    date: October 29, 2018

    Takes in a skin and an animation and plays the animation for a
    given mesh and shader.
**********************************************************************/

#include "mesh_renderer.h"
#include "Time/clock.h"
#include "Animation/skeleton.h"
#include "Animation/animation_clip.h"

namespace Components {

    //**********************************************************************
    class SkinnedMeshRenderer : public MeshRenderer
    {
    public:
        //----------------------------------------------------------------------
        // Creates a new skinned mesh renderer which updates skinning matrices from
        // a given animation and skeleton. Note that the shader from the given material
        // should be suitable for skinning. Component warns if that is not the case.
        //----------------------------------------------------------------------
        SkinnedMeshRenderer(const MeshPtr& mesh, const Animation::Skeleton& skeleton, const Animation::AnimationClip& animation, const MaterialPtr& material = nullptr);

        //----------------------------------------------------------------------
        // Updates current played animation.
        //----------------------------------------------------------------------
        void tick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        // Start playing the given animation
        //----------------------------------------------------------------------
        void playAnimation(const Animation::AnimationClip& animation);

        //----------------------------------------------------------------------
        Time::Clock& getClock() { return m_clock; }
        const ArrayList<DirectX::XMMATRIX>& getMatrixPalette()      const { return m_matrixPalette; }
        const ArrayList<DirectX::XMMATRIX>& getJointWorldMatrices() const { return m_jointWorldMatrices; }

    private:
        Time::Clock m_clock{ 1000_ms }; // By default same duration as a given animation

        ArrayList<DirectX::XMMATRIX>    m_matrixPalette;
        ArrayList<DirectX::XMMATRIX>    m_jointWorldMatrices;
        Animation::Skeleton             m_skeleton;
        Animation::AnimationClip        m_animation;

        DirectX::XMVECTOR _GetInterpolatedTranslation(U32 joint, Time::Seconds clockTime);
        DirectX::XMVECTOR _GetInterpolatedRotation(U32 joint, Time::Seconds clockTime);
        DirectX::XMVECTOR _GetInterpolatedScale(U32 joint, Time::Seconds clockTime);

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;

        NULL_COPY_AND_ASSIGN(SkinnedMeshRenderer)
    };

}