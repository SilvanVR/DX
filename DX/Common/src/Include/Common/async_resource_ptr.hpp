#pragma once
/**********************************************************************
    class: AsyncResourcePtr (async_resource_ptr.hpp)

    author: S. Hau
    date: April 8, 2018

    Class which acts similar to a std::shared_ptr(), but has some
    extended capabilities, like calling a function whenever the
    underlying pointer changed.
**********************************************************************/

template <typename T>
class AsyncResourcePtr
{
    typedef void(*Deleter)(T*);


public:
    //----------------------------------------------------------------------
    AsyncResourcePtr()  { m_data = new AsyncResourceData(); }
    ~AsyncResourcePtr() { _DecrementReference(); delete m_data; }

    //----------------------------------------------------------------------
    void reset(T* resource)
    {
        _DecrementReference();
        m_data->ptr = resource;
        _IncrementReference();
        if (m_resourceChangedCallback)
            m_resourceChangedCallback( resource );
    }

    //----------------------------------------------------------------------
    void setDeleteCallback(Deleter deleter)
    {
        m_deleteCallback = deleter;
    }

    //----------------------------------------------------------------------
    void setResourceChangedCallback(const std::function<void(T*)>& cb)
    {
        ASSERT( m_resourceChangedCallback == nullptr );
        m_resourceChangedCallback = cb;
    }

    //U32 getRefCount() const { return m_data->refCount; }

    //----------------------------------------------------------------------
    bool isValid() const { return (m_data->ptr != nullptr); }

    bool operator==(std::nullptr_t null) const { return !isValid(); }
    bool operator!=(std::nullptr_t null) const { return isValid(); }

    T*          get()       { ASSERT( isValid() ); return m_data->ptr; }
    const T*    get() const { ASSERT( isValid() ); return m_data->ptr; }

    const T* operator->() const { ASSERT( isValid() ); return m_data->ptr; }
    T*       operator->()       { ASSERT( isValid() ); return m_data->ptr; }

private:
    struct AsyncResourceData
    {
        I32 refCount    = 0;
        T*  ptr         = nullptr;
    };

    AsyncResourceData*          m_data                      = nullptr;
    std::function<void(T*)>     m_resourceChangedCallback   = nullptr;
    Deleter                     m_deleteCallback            = [](T* res) { delete res; };

    //----------------------------------------------------------------------
    inline void _IncrementReference() 
    { 
        m_data->refCount++; 
    }

    //----------------------------------------------------------------------
    inline void _DecrementReference() 
    {
        if ( isValid() )
        {
            m_data->refCount--; 
            if ( m_data->refCount == 0 )
                m_deleteCallback( m_data->ptr );
        }
    }

    //----------------------------------------------------------------------
    AsyncResourcePtr(const AsyncResourcePtr& other) = delete;
    AsyncResourcePtr& operator = (const AsyncResourcePtr& other) = delete;
    AsyncResourcePtr(AsyncResourcePtr&& other) = delete;
    AsyncResourcePtr& operator = (AsyncResourcePtr&& other) = delete;
};