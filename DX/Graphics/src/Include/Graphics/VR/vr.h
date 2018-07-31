#pragma once
/**********************************************************************
    class: VRDevice

    author: S. Hau
    date: July 22, 2018

    Super class for all HMD devices.
**********************************************************************/

#include "../structs.hpp"
#include "../enums.hpp"

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

    //----------------------------------------------------------------------
    // @Return: The first supported HMD on this system.
    //----------------------------------------------------------------------
    Device GetFirstSupportedHMD();

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

        //----------------------------------------------------------------------
        // @Return: Whether this HMD was sucessfully initialized.
        //----------------------------------------------------------------------
        bool isInitialized() const { return m_initialized; }

        //----------------------------------------------------------------------
        // @Return: Whether the last frame was actually displayed e.g. if the application lost focus
        //----------------------------------------------------------------------
        bool isVisible() const { return m_isVisible; }

        //----------------------------------------------------------------------
        // @Return: True, when app has focus e.g. oculus dash is not opened.
        //----------------------------------------------------------------------
        virtual bool hasFocus() = 0;

        //----------------------------------------------------------------------
        // @Return: Calculates and returns the current eye poses.
        //----------------------------------------------------------------------
        virtual std::array<EyePose, 2> calculateEyePoses(I64 frameIndex) = 0;

        //----------------------------------------------------------------------
        // @Return: Last calculated eye poses from calculateEyePoses().
        //----------------------------------------------------------------------
        virtual std::array<EyePose, 2> getEyePoses() const = 0;

        //----------------------------------------------------------------------
        // @Return: Struct describing current touch state.
        //----------------------------------------------------------------------
        const Touch&                getTouch(Hand hand) const { return m_touch[(I32)hand]; }
        const std::array<Touch, 2>& getTouch()          const { return m_touch; }

    protected:
        HMDDescription          m_description;
        bool                    m_initialized = false;
        bool                    m_isVisible = true;
        std::array<Touch, 2>    m_touch;

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