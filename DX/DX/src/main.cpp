

#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/include.hpp"
#include "Core/subsystem_manager.h"

#include "Core/OS/FileSystem/file.h"
#include "Core/OS/Threading/thread_pool.h"
#include <thread>


using namespace Core;

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
        //LOG("Constructor");
    }
    ~A() {
        int i = 25;
        //LOG("Destructor");
    }

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
        time = other.time;
    }

    A& operator=(const A&& other)
    {
        LOG("Move Assignment");
        time = other.time;
        return *this;
    }


    //static void* operator new(Size sz)
    //{
    //    std::cout << "custom new for size " << sz << '\n';
    //    return gPoolAllocator.allocateRaw( sz, alignof(A) );
    //}

    //static void operator delete(void* mem)
    //{
    //    std::cout << "custom delete \n";
    //    return gPoolAllocator.deallocate( mem );
    //}

    //static void* operator new[](std::size_t sz)
    //{
    //    std::cout << "custom new[] for size " << sz << '\n';
    //    return gPoolAllocator.allocateRaw(sz, alignof(A));
    //}

    //static void operator delete[](void* mem)
    //{
    //    std::cout << "custom delete[] \n";
    //    return gPoolAllocator.deallocate(mem);
    //}


    OS::SystemTime time;
};

class B : public A
{
public:
    U32 lols;
};

//@TODO: 
// - Profiler
// Threading -> async file reading/writing


void func(int i)
{
    LOG( "Hello " + TS(i) , Color::RED );
}

int main(void)
{
    //MemoryManagement::UniversalAllocator gUniversalAllocator(1000);
    //StringID te = SID("Hello");
    //StringID te2 = SID("World");
    //LOG(te.toString() + " " + te2.toString());

    Core::SubSystemManager gSubSystemManager;
    gSubSystemManager.init();
    Locator::getLogger().setSaveToDisk( false );

    {
        //U32 numThreads = std::thread::hardware_concurrency();
        //std::thread thread(func, 42);
        ////LOG("TEST");
        //thread.join();

        OS::ThreadPool threadPool(7);
        //OS::Thread& thread = threadPool[0];

        threadPool.addJob([] {
            LOG( "NEW JOB!", Color::BLUE );
        }, [] {
            LOG( "NEW JOB IS DONE" );
        });

        threadPool.addJob([] {
            LOG("ANOTHER JOB!", Color::RED);
        });

        threadPool.addJob([] {
            LOG("A super awesome job", Color::RED);
        });

        //threadPool.waitForThreads();
    }

    {
        //OS::TextFile file("test.txt", OS::EFileMode::READ_WRITE_OVERWRITE);
        //file.readAsync([] (err, data) {
        //    LOG( "File was read!" );
        //});
    }

    {
        //AutoClock clock;

        //LOG( "Hello World" );
        //LOG( "IMPORTANT", Logging::LOG_LEVEL_IMPORTANT, Color::BLUE );
        //WARN( "NOT SO IMPORTANT", Logging::LOG_LEVEL_NOT_SO_IMPORTANT );

        //LOG( SID("Hello") );
        //Color color( 16, 52, 128, 255);
        //LOG( color.toString(true), Color::RED );
    }


    gSubSystemManager.shutdown();
    system("pause");

    return 0;
}
