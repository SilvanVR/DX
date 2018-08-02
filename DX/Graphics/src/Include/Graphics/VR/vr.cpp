#include "vr.h"
/**********************************************************************
    class: VRDevice

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "OculusRift/LibOVR/OVR_CAPI.h"
#include "Logging/logging.h"

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    void UserLogCallbackOculus( uintptr_t userData, int level, const char* message )
    {
        switch (level)
        {
        case 0: LOG_RENDERING( message ); break;
        case 1: // Fall through
        case 2: LOG_WARN_RENDERING( message ); break;
        default:
            LOG_RENDERING( message );
        }
    }

    //----------------------------------------------------------------------
    Device GetFirstSupportedHMDAndInitialize()
    {
        // Oculus Rift
        {
            ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, UserLogCallbackOculus, 0, 0 };
            ovrResult result = ovr_Initialize( &initParams );
            if (result == ovrSuccess)
                return Device::OculusRift;
        }

        return Device::Unknown;
    }


} } // End namespaces