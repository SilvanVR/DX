#pragma once
/**********************************************************************
    class: VRDevice

    author: S. Hau
    date: July 22, 2018

    Super class for all HMD devices.
**********************************************************************/

#include "../structs.hpp"
#include "../enums.hpp"
#include <functional>

namespace Graphics { class D3D11Renderer; class VulkanRenderer; }

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    struct HMDDescription
    {
        String name;                        // Name from the headset
        Math::Vec2Int resolution;           // Hardware resolution for both eyes
        Math::Vec2Int idealResolution[2];   // Ideal buffer resolution for EACH eye
        Device device;                      // Which HMD as enum
    };

    //----------------------------------------------------------------------
    struct Pose
    {
        Math::Vec3 position;
        Math::Quat rotation;
    };

    //----------------------------------------------------------------------
    // @Return: The first supported HMD on this system and initializes the corresponding library.
    //----------------------------------------------------------------------
    Device GetFirstSupportedHMDAndInitialize();

    //**********************************************************************
    // Abstraction for an HMD plus touch controller.
    //**********************************************************************
    class HMD
    {
    public:
        HMD() = default;
        virtual ~HMD() = default;

        //----------------------------------------------------------------------
        const HMDDescription&   getDescription()            const { return m_description; }
        F32                     getWorldScale()             const { return m_worldScale; }

        //----------------------------------------------------------------------
        // @Params:
        //  "frameIndex": Used if rendering happens on another thread than the main thread.
        // @Return:
        //  Current eye poses for both eyes.
        //----------------------------------------------------------------------
        const std::array<Pose, 2>& getEyePoses(I64 frameIndex);

        //----------------------------------------------------------------------
        // @Params:
        //  "frameIndex": Used if rendering happens on another thread than the main thread.
        // @Return:
        //  Current touch poses for both touch controller.
        //----------------------------------------------------------------------
        const Pose& getTouchPose(Hand hand, I64 frameIndex);
        const std::array<Pose, 2>& getTouchPoses(I64 frameIndex);

        //----------------------------------------------------------------------
        // @Return: True, when app has focus e.g. oculus dash is not opened.
        //----------------------------------------------------------------------
        bool hasFocus() const { return m_hasFocus; }

        //----------------------------------------------------------------------
        // @Return: False whenever something is wrong with this hmd.
        //----------------------------------------------------------------------
        virtual bool good() const = 0;

        //----------------------------------------------------------------------
        // @Return: Projection matrix for the given eye.
        //----------------------------------------------------------------------
        virtual DirectX::XMMATRIX getProjection(Eye eye, F32 zNear, F32 zFar) const = 0;

        //----------------------------------------------------------------------
        // Disables/Enables the performance hud with the given mode.
        //----------------------------------------------------------------------
        virtual void setPerformanceHUD(PerfHudMode mode) = 0;

        //----------------------------------------------------------------------
        // @Return: True when the user wears the headset.
        //----------------------------------------------------------------------
        virtual bool isMounted() = 0;

        //----------------------------------------------------------------------
        // Changes the world scale. A larger values corresponds to the perception of a larger world.
        //----------------------------------------------------------------------
        void setWorldScale(F32 newWorldScale) { m_worldScale = newWorldScale; }

    protected:
        HMDDescription          m_description;
        bool                    m_hasFocus = true;
        std::array<Pose, 2>     m_currentEyePoses;
        std::array<Pose, 2>     m_currentTouchPoses;
        F32                     m_worldScale = 1.0f;
        I64                     m_currentFrameIndex = -1; // Used to keep track the last time poses were calculated

    private:

        friend class D3D11Renderer;
        friend class VkRenderer;

        //----------------------------------------------------------------------
        // Clears the next swapchain texture for both eyes with the given color.
        //----------------------------------------------------------------------
        virtual void clear(Color col) = 0;

        //----------------------------------------------------------------------
        // Distorts the swapchain images and presents them to the HMD.
        //----------------------------------------------------------------------
        virtual void distortAndPresent(I64 frameIndex) = 0;

        //----------------------------------------------------------------------
        // Bind the next texture in the swapchain to the OM for the given eye.
        //----------------------------------------------------------------------
        virtual void bindForRendering(Eye eye) = 0;

        //----------------------------------------------------------------------
        // Updates the current eye & touch-poses.
        //----------------------------------------------------------------------
        virtual void _UpdateEyeAndTouchPoses(I64 frameIndex) = 0;

        NULL_COPY_AND_ASSIGN(HMD)
    };

} } // End namespaces