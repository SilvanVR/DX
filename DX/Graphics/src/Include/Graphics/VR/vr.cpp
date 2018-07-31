#include "vr.h"
/**********************************************************************
    class: VRDevice

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "LibOVR/OVR_CAPI.h"

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    Device GetFirstSupportedHMD()
    {
        ovrResult result = ovr_Initialize( nullptr );
        if (result == ovrSuccess)
            return Device::OculusRift;

        return Device::Unknown;
    }


} } // End namespaces