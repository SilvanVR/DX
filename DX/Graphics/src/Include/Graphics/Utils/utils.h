#pragma once
/**********************************************************************
    class: None (utils.h)

    author: S. Hau
    date: March 31, 2018
**********************************************************************/

#include "enums.hpp"

namespace Graphics {

    //----------------------------------------------------------------------
    // @Return:
    //  Byte-Count for the given texture format.
    //----------------------------------------------------------------------
    U32 ByteCountFromTextureFormat( TextureFormat format );

}
