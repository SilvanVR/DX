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
    //using clock = std::chrono::high_resolution_clock;
    //auto timePoint1 = clock::now();
    //Sleep(SLEEP);
    //auto timePoint2 = clock::now();
    //double diff = std::chrono::duration<double>(timePoint2 - timePoint1).count();
    //LOG(diff);

    {
        AutoClock clock;
        //Sleep(SLEEP);
       /* int j = 0;
        for (int i = 0; i < 1000000000; i++)
            j += i;*/
    }

    system("pause");
    return 0;
}