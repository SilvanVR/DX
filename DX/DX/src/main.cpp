#include <iostream>
#include <chrono>
#include <windows.h>
#include <string>

#include "data_types.hpp"
#include "Time/platform_timer.h"

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


int main(void)
{
    {
        AutoClock clock;
        Sleep(SLEEP);
       /* int j = 0;
        for (int i = 0; i < 1000000000; i++)
            j += i;*/
    }

    auto sysTime = PlatformTimer::get()->getCurrentTime();
    Sleep(SLEEP);
    auto sysTime2 = PlatformTimer::get()->getCurrentTime();

    bool less = sysTime < sysTime2;
    bool gr = sysTime > sysTime2;
    bool eq = sysTime == sysTime2;

    system("pause");
    return 0;
}