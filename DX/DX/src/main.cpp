#include <iostream>

#include "OS/PlatformTimer/platform_timer.h"

#define LOG(x) std::cout << x << std::endl

class AutoClock
{
    I64 begin;

public:
    AutoClock()
    {
        begin = OS::PlatformTimer::getTicks();
    }

    ~AutoClock()
    {
        I64 elapsedTicks = OS::PlatformTimer::getTicks() - begin;

        F64 elapsedSeconds = OS::PlatformTimer::ticksToSeconds(elapsedTicks);
        LOG("Seconds: " + TS(elapsedSeconds));

        F64 elapsedMillis = OS::PlatformTimer::ticksToMilliSeconds(elapsedTicks);
        LOG("Millis: " + TS(elapsedMillis));

        F64 elapsedMicros = OS::PlatformTimer::ticksToMicroSeconds(elapsedTicks);
        LOG("Micros: " + TS(elapsedMicros));

        F64 elapsedNanos = OS::PlatformTimer::ticksToNanoSeconds(elapsedTicks);
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


void func(StringID name)
{
    LOG("The objects name is: " + IDS(name));
}


int main(void)
{
    StringID te = SID("Hello");
    StringID te2 = SID("World");

    String ste = IDS(te);
    String ste2 = IDS(te2);

    LOG(ste + " " + ste2);

    StringID martin = SID("Martin");
    func(martin);


    SystemTime curTime = OS::PlatformTimer::getCurrentTime();
    LOG(curTime.toString());

    system("pause");
    return 0;
}

