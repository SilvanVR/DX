#pragma once
/**********************************************************************
    class: OculusRift

    author: S. Hau
    date: July 22, 2018
**********************************************************************/

#include "LibOVR/OVR_CAPI.h"
#include "../enums.hpp"

namespace Graphics { namespace VR {

    //**********************************************************************
    class OculusRift
    {
    public:
        OculusRift(API api);
        ~OculusRift();

    private:
        ovrSession m_session;

        NULL_COPY_AND_ASSIGN(OculusRift)
    };

} } // End namespaces