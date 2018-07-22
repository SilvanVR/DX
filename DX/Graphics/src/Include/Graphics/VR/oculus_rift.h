#pragma once
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "LibOVR/OVR_CAPI.h"

namespace Graphics { namespace VR {

    //**********************************************************************
    class OculusRift
    {
    public:
        OculusRift();
        ~OculusRift();

    private:
        ovrSession m_session;

        NULL_COPY_AND_ASSIGN(OculusRift)
    };

} } // End namespaces