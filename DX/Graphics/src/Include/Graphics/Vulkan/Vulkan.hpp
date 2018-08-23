#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#include "VkPlatform.h"

extern Graphics::Vulkan::Platform g_vulkan;

#define VK_RELEASE(x) if(x){ x->Release(); x = nullptr; }
