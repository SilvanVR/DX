#include "oculus_rift.h"
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics { namespace VR {

    #define CHECKVR(result, msg)  if (result != ovrSuccess) \
    {\
        LOG_WARN_RENDERING( msg );\
        return;\
    }\

    //----------------------------------------------------------------------
    OculusRift::OculusRift()
    {
        ovrResult result = ovr_Initialize( nullptr );
        CHECKVR( result, "OculusRift: Failed to initialize libOVR." );

        ovrGraphicsLuid luid;
        result = ovr_Create( &m_session, &luid );
        CHECKVR( result, "OculusRift: HMD not detected" );
        ovrHmdDesc HMDInfo = ovr_GetHmdDesc( m_session );
    }

    //----------------------------------------------------------------------
    OculusRift::~OculusRift()
    {
        if (m_session)
        {
            ovr_Destroy( m_session );
            ovr_Shutdown();
        }
    }

} } // End namespaces