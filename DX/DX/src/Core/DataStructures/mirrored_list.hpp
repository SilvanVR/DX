#pragma once
/**********************************************************************
    class: MirroredList (mirrored_list.hpp)

    author: S. Hau
    date: December 17, 2017

    Represents an abstract data-structure, which consists of three lists:
     - One list for active objects
     - One list for non active objects
     - One list for all objects
    This is useful for objects, which can be enabled/disabled.
    Disable objects won't waste performance then.

    Requires two functions on the template type:
     1.) bool isActive()
     2.) void setActive(bool)

     NOT USED ANYMORE

    A far better way to achieve this:
     - Have inactive objects at the end of a SINGLE list and swap
       data if one become inactive -> Just store the amount of "active" objects
**********************************************************************/

namespace Core {

    //**********************************************************************
    template<typename T>
    class MirroredList
    {
    public:
        MirroredList() = default;

        const ArrayList<T>& ActiveObjects() const { return m_activeObjects; }
        const ArrayList<T>& NonActiveObjects() const { return m_nonActiveObjects; }

        void add(T obj);
        void enable(T obj);
        void disable(T obj);

        // Enable range-based for-loop
        T* begin(){ return &m_objects.front(); }
        T* end(){ return &m_objects[m_objects.size()]; }

    private:
        ArrayList<T> m_objects;
        ArrayList<T> m_activeObjects;
        ArrayList<T> m_nonActiveObjects;

        //----------------------------------------------------------------------
        MirroredList(const MirroredList& other)               = delete;
        MirroredList& operator = (const MirroredList& other)  = delete;
        MirroredList(MirroredList&& other)                    = delete;
        MirroredList& operator = (MirroredList&& other)       = delete;
    };

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    template<typename T>
    void MirroredList<T>::add( T obj )
    {
        m_objects.push_back( obj );
        if ( obj->isActive() )
            m_activeObjects.push_back( obj );
        else
            m_nonActiveObjects.push_back( obj );
    }

    //----------------------------------------------------------------------
    template<typename T>
    void MirroredList<T>::enable( T obj )
    {
        if( obj->isActive() )
            return;
    }

    //----------------------------------------------------------------------
    template<typename T>
    void MirroredList<T>::disable( T obj )
    {
        if ( !obj->isActive() )
            return;
    }

} // End namespaces
