#pragma once
/**********************************************************************
    class: None (utils.h)

    author: S. Hau
    date: October 3, 2017

    Some utility functions, which dont belong to anywhere else.
**********************************************************************/

#include "data_types.hpp"

namespace Utils
{


    //**********************************************************************
    // Converts amount of bytes to a nice and readable string.
    // @Params:
    // "bytes": Amount of bytes
    // "binaryPrefix": If true, conversion to kibibytes/mibibytes/gibibytes
    //**********************************************************************
    String bytesToString(U64 bytes, bool binaryPrefix = false);


    //**********************************************************************
    // Converts a pointer to a memory address in hexadecimal form e.g.
    // 0x00...00
    // @Params:
    // "address": Pointer to the memory location.
    //**********************************************************************
    String memoryAddressToString(void* address);


    //**********************************************************************
    // @Return:
    //  Size in bytes for the corresponding type. 0 if not valid for the given type.
    //**********************************************************************
    Size GetSizeInBytesOfType(DataType dataType);

}