#pragma once
/**********************************************************************
    class: VRCamera

    author: S. Hau
    date: August 1, 2018
**********************************************************************/

#include "../i_component.h"
#include "Events/event.hpp"
#include "Graphics/enums.hpp"

namespace Components
{
    class Camera;

    //----------------------------------------------------------------------
    enum class ScreenDisplay // What will be displayed on the screen
    {
        LeftEye,
        RightEye,
        BothEyes,
        None
    };

    //**********************************************************************
    // Consists of two basic camera components, which renders into each eye.
    //**********************************************************************
    class VRCamera : public IComponent
    {
    public:
        VRCamera(ScreenDisplay screenDisplay = ScreenDisplay::LeftEye, Graphics::MSAASamples sampleCount = Graphics::MSAASamples::Four, bool hdr = false)
            : m_sampleCount(sampleCount), m_hdr(hdr), m_screenDisplay(screenDisplay) {}
        ~VRCamera() = default;

        //----------------------------------------------------------------------
        // Changes what will be display on the regular screen/monitor (not the hmd).
        //----------------------------------------------------------------------
        void setScreenDisplay(ScreenDisplay sd);

        //----------------------------------------------------------------------
        // @Return: Direction in which the HMD is facing in World Space
        //----------------------------------------------------------------------
        Math::Vec3 getLookDirection();

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;
        void setActive(bool active);

    private:
        GameObject*             m_eyeGameObjects[2];
        Components::Camera*     m_eyeCameras[2];
        Events::ListenerID      m_frameEvtListenerID;
        Graphics::MSAASamples   m_sampleCount;
        ScreenDisplay           m_screenDisplay;
        bool                    m_hdr;

        void _FrameBegin();

        NULL_COPY_AND_ASSIGN(VRCamera)
    };


}