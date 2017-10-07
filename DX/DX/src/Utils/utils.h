#pragma once


namespace Utils
{


    //**********************************************************************
    // Converts amount of bytes to a nice and readable string.
    // @Params:
    // "bytes": Amount of bytes
    // "binaryPrefix": If true, conversion to kibibytes/mibibytes/gibibytes
    //**********************************************************************
    String bytesToString(U64 bytes, bool binaryPrefix = false);




}