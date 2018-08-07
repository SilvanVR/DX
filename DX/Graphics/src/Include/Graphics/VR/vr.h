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
    struct EyePose
    {
        Math::Vec3 position;
        Math::Quat rotation;
        DirectX::XMMATRIX projection;
    };

    //----------------------------------------------------------------------
    struct Touch
    {
        Math::Vec3 position;
        Math::Quat rotation;
    };

    using HMDCallback   = std::function<void(Eye eye, const EyePose&)>;
    using TouchCallback = std::function<void(const Touch&)>;

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
        const HMDDescription& getDescription() const { return m_description; }
        F32 getWorldScale() const { return m_worldScale; }

        //----------------------------------------------------------------------
        // @Return: True, when app has focus e.g. oculus dash is not opened.
        //----------------------------------------------------------------------
        bool hasFocus() { return m_hasFocus; }

        //----------------------------------------------------------------------
        // Calculates the current eye poses and calls the registered callbacks.
        // @Params:
        //  "frameIndex": Used if rendering happens on another thread than the main thread.
        //----------------------------------------------------------------------
        virtual void calculateEyePosesAndTouch(I64 frameIndex) = 0;

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

        //----------------------------------------------------------------------
        // Set the eye callback for retrieving position, orientation & projection.
        //----------------------------------------------------------------------
        void setHMDCallback(const HMDCallback& cb) { m_hmdCallback = cb; }

        //----------------------------------------------------------------------
        // Set the touch callback for retrieving position & orientation.
        //----------------------------------------------------------------------
        void setTouchCallback(Hand hand, const TouchCallback& cb) { m_touchCallbacks[(I32)hand] = cb; }

    protected:
        HMDDescription          m_description;
        F32                     m_worldScale = 1.0f;
        HMDCallback             m_hmdCallback;
        TouchCallback           m_touchCallbacks[2];
        bool                    m_hasFocus = true;

    private:
        friend class D3D11Renderer;
        friend class VulkanRenderer;

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

        NULL_COPY_AND_ASSIGN(HMD)
    };

} } // End namespaces