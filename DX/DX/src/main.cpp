#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

#include "data_types.hpp"
#include "PlatformTimer/platform_timer.h"
#include "DataStructures/ArrayList.hpp"

#define LOG(x) std::cout << x << std::endl
#define SLEEP 10
#define TS(x) std::to_string(x)

class AutoClock
{
    I64 begin;

public:
    AutoClock()
    {
        begin = PlatformTimer::get()->getTicks();
    }

    ~AutoClock()
    {
        I64 elapsedTicks = PlatformTimer::get()->getTicks() - begin;

        F64 elapsedSeconds = PlatformTimer::get()->ticksToSeconds(elapsedTicks);
        LOG("Seconds: " + TS(elapsedSeconds));

        F64 elapsedMillis = PlatformTimer::get()->ticksToMilliSeconds(elapsedTicks);
        LOG("Millis: " + TS(elapsedMillis));

        F64 elapsedMicros = PlatformTimer::get()->ticksToMicroSeconds(elapsedTicks);
        LOG("Micros: " + TS(elapsedMicros));

        F64 elapsedNanos = PlatformTimer::get()->ticksToNanoSeconds(elapsedTicks);
        LOG("Nanos: " + TS(elapsedNanos));
    }
};


class A
{
public:
    A() {
        LOG("Constructor");    
    }
    ~A() {}

    A(const A& other)
    {
        LOG("Copy Constructor");
    }

    A& operator=(const A& other)
    {
        LOG("Assignment Operator");
        return *this;
    }

    A(const A&& other)
    {
        LOG("Move Constructor");
    }

    A& operator=(const A&& other)
    {
        LOG("Move Assignment");
        return *this;
    }
private:
    SystemTime data;
};

class B : public A
{

};

#define SIZE 5

// add subclass to vector? Necessary?

int main(void)
{
    std::vector<A> list;
    list.reserve(SIZE);

    A a;

    {
        AutoClock clock;
        for (int i = 0; i < SIZE; i++)
            list.push_back(A());
    }

    ArrayList<A> list2(SIZE);
    {
        AutoClock clock;
        for (int i = 0; i < SIZE; i++)
            list2.add(A());
    }

    system("pause");
    return 0;
}