#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "Logging/logging.h"

//----------------------------------------------------------------------
#define ALLOCATOR nullptr

#ifdef _DEBUG
    String ResultToString(VkResult result);

    #define VALIDATE(x) \
        if ( x != VK_SUCCESS ) { \
            LOG_ERROR_RENDERING( String( "VkRenderer: @" ) + __FILE__ + ", line " + TS(__LINE__) + ". "\
                                 "Function: " + #x + ". Reason: " + ResultToString(x) );\
        }
#else
    #define VALIDATE(x) (x)
#endif

template <typename T>
class VkPtr
{
public:
    VkPtr(T* data = nullptr) : m_data(data) {}
    ~VkPtr() { release(); }

    T*&         get()       { return m_data; }
    const T*&   get() const { return m_data; }

    operator T*&() { return m_data; }

    T*          operator-> ()       { return m_data; }
    const T*    operator-> () const { return m_data; }

    T*&         releaseAndGet()     { release(); return m_data; }

    void release() { SAFE_RELEASE(m_data); }

private:
    T* m_data = nullptr;

    NULL_COPY_AND_ASSIGN(VkPtr)
};