#pragma once

/**********************************************************************
    class: Singleton (singleton.hpp)

    author: S. Hau
    date: September 9, 2017

    Interface for an singleton object. Capable of returning a 
    static instance of the templated class.
**********************************************************************/

template<class T>
class Singleton
{
public:
    Singleton() {}
    virtual ~Singleton() = 0 {}

    static T* getSingleton() { return &m_Instance; }
    static T* get() { return getSingleton(); }

protected:
    static T m_Instance;

private:
    // Forbid copy + copy assignment and rvalue copying
    Singleton (const Singleton& other) = delete;
    Singleton& operator = (const Singleton& other) = delete;
    Singleton (const Singleton&& other) = delete;
    Singleton& operator = (const Singleton&& other) = delete;
};

template <class T>
T Singleton<T>::m_Instance;
