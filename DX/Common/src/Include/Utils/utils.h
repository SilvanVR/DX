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


    //**********************************************************************
    // Converts a pointer to a memory address in hexadecimal form e.g.
    // 0x00...00
    // @Params:
    // "address": Pointer to the memory location.
    //**********************************************************************
    String memoryAddressToString(void* address);


}