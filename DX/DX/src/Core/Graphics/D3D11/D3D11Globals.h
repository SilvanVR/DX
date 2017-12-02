#pragma once
/**********************************************************************
    class: None (D3D11Globals.h)

    author: S. Hau
    date: December 2, 2017

    Defines the global used device and device context.
**********************************************************************/

#include "D3D11.hpp"

ID3D11Device*           g_pDevice           = nullptr;
ID3D11DeviceContext*    g_pImmediateContext = nullptr;