//#pragma once
///**********************************************************************
//    class: AsyncResourcePtr (async_resource_ptr.hpp)
//
//    author: S. Hau
//    date: April 8, 2018
//
//    Class which acts similar to a std::shared_ptr(), but has some
//    extended capabilities, like calling a function whenever the
//    underlying pointer changed.
//**********************************************************************/
//
//#include <functional>
//
//template <typename T>
//class AsyncResourcePtr
//{
//    typedef void(*Deleter)(T*);
//
//public:
//    //----------------------------------------------------------------------
//    AsyncResourcePtr(std::nullptr_t)    { m_data = new AsyncResourceData(); }
//    AsyncResourcePtr()                  { m_data = new AsyncResourceData(); }
//    ~AsyncResourcePtr()                 { _DecrementReference(); }
//
//    U32     getRefCount()   const { return m_data->refCount; }
//    bool    isUnique()      const { return getRefCount() == 1; }
//
//    //----------------------------------------------------------------------
//    void reset(T* resource)
//    {
//        _DecrementReference();
//        m_data->ptr = resource;
//        _IncrementReference();
//        if (m_resourceChangedCallback)
//            m_resourceChangedCallback( resource );
//    }
//
//    //----------------------------------------------------------------------
//    void setDeleteCallback(Deleter deleter)
//    {
//        m_deleteCallback = deleter;
//    }
//
//    //----------------------------------------------------------------------
//    void setResourceChangedCallback(const std::function<void(T*)>& cb)
//    {
//        ASSERT( m_resourceChangedCallback == nullptr );
//        m_resourceChangedCallback = cb;
//    }
//
//    //----------------------------------------------------------------------
//    bool isValid() const { return (m_data->ptr != nullptr); }
//
//    bool        operator== (std::nullptr_t null) const { return !isValid(); }
//    bool        operator!= (std::nullptr_t null) const { return isValid(); }
//
//    T*          get()       { ASSERT( isValid() ); return m_data->ptr; }
//    const T*    get() const { ASSERT( isValid() ); return m_data->ptr; }
//
//    const T*    operator->() const { ASSERT( isValid() ); return m_data->ptr; }
//    T*          operator->()       { ASSERT( isValid() ); return m_data->ptr; }
//
//    //----------------------------------------------------------------------
//    AsyncResourcePtr(AsyncResourcePtr& other)
//    {
//        this->_CopyConstructFrom( other );
//    }
//
//    //----------------------------------------------------------------------
//    AsyncResourcePtr(AsyncResourcePtr&& other)
//    {
//        this->_MoveConstructFrom( std::move( other ) );
//    }
//
//    //----------------------------------------------------------------------
//    template <class T2, class = std::enable_if<std::is_convertible<T2*, T*>::value, void>::type>
//    AsyncResourcePtr(const AsyncResourcePtr<T2>& other)
//    {
//        m_data                      = dynamic_cast<T*>( other.m_data );
//        m_resourceChangedCallback   = other.m_resourceChangedCallback;
//        m_deleteCallback            = other.m_deleteCallback;
//        _IncrementReference();
//    }
//
//    //----------------------------------------------------------------------
//    AsyncResourcePtr& operator=(AsyncResourcePtr& other)
//    {
//        _DecrementReference();
//        this->_CopyConstructFrom( other );
//        return *this;
//    }
//
//    //----------------------------------------------------------------------
//    AsyncResourcePtr& operator = (AsyncResourcePtr&& other)
//    {
//        _DecrementReference();
//        this->_MoveConstructFrom( std::move( other ) );
//        return *this;
//    }
//
//
//private:
//    struct AsyncResourceData
//    {
//        I32 refCount    = 0;
//        T*  ptr         = nullptr;
//    };
//
//    AsyncResourceData*          m_data                      = nullptr;
//    std::function<void(T*)>     m_resourceChangedCallback   = nullptr;
//    Deleter                     m_deleteCallback            = [](T* res) { delete res; };
//
//    //----------------------------------------------------------------------
//    inline void _CopyConstructFrom(AsyncResourcePtr& other)
//    {
//        m_data                      = other.m_data;
//        m_resourceChangedCallback   = other.m_resourceChangedCallback;
//        m_deleteCallback            = other.m_deleteCallback;
//        _IncrementReference();
//    }
//
//    //----------------------------------------------------------------------
//    inline void _MoveConstructFrom(AsyncResourcePtr&& other)
//    {
//        m_data                      = other.m_data;
//        m_resourceChangedCallback   = other.m_resourceChangedCallback;
//        m_deleteCallback            = other.m_deleteCallback;
//
//        other.m_data                    = nullptr;
//        other.m_resourceChangedCallback = nullptr;
//        other.m_deleteCallback          = [](T* res) { delete res; };
//
//        _IncrementReference();
//    }
//
//    //----------------------------------------------------------------------
//    inline void _IncrementReference() 
//    { 
//        m_data->refCount++; 
//    }
//
//    //----------------------------------------------------------------------
//    inline void _DecrementReference() 
//    {
//        if ( isValid() )
//        {
//            m_data->refCount--; 
//            if ( m_data->refCount == 0 )
//            {
//                m_deleteCallback( m_data->ptr );
//                delete m_data;
//            }
//        }
//    }
//};