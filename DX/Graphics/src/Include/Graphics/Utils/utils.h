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
    U32 ByteCountFromTextureFormat(TextureFormat format);

    //----------------------------------------------------------------------
    // Converts a string back to a render-queue integer
    // @Return:
    //  Render-Queue which represents the given string. -1 if string is not predefined renderqueue.
    //----------------------------------------------------------------------
    I32 StringToRenderQueue(const String& str);

    //----------------------------------------------------------------------
    // @Return: Shadertype as a readable string.
    //----------------------------------------------------------------------
    String GetShaderTypeName(ShaderType shaderType);

    //----------------------------------------------------------------------
    // @Return: Whether given format is a depth format
    //----------------------------------------------------------------------
    bool IsDepthFormat(TextureFormat format);

}
