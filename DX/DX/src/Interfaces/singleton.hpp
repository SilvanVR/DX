#pragma once

/**********************************************************************
    class: Singleton (singleton.hpp)

    author: S. Hau
    date: September 9, 2017

    Interface for an singleton object. Constructs an static instance
    of the template type on the stack on the first call of
    "get" or "getSingleton".
**********************************************************************/

template<class T>
class Singleton
{
public:
    Singleton() {}
    virtual ~Singleton() = 0 {}

    static T* getSingleton() 
    {
        static T singletonInstance;

        return &singletonInstance;
    }

    static T* get() { return getSingleton(); }

private:
    // Forbid copy + copy assignment and rvalue copying
    Singleton (const Singleton& other) = delete;
    Singleton& operator = (const Singleton& other) = delete;
    Singleton (const Singleton&& other) = delete;
    Singleton& operator = (const Singleton&& other) = delete;
};
