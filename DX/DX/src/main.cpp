

#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/include.hpp"
#include "Core/subsystem_manager.h"
#include "Core/OS/FileSystem/file_system.h"

#include "Core/Config/config_file.h"

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
// Binary-File!!!
// Threading -> async file reading/writing

// Read until specific character has been reached
// jump to line

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
        Config::ConfigFile configFile("/config/engine.ini");

        //float resX = configFile["General"]["ResolutionX"];
        //float resY = configFile["General"]["ResolutionY"];

        U32 x = configFile["General"]["ResolutionX"];
        U32 y = configFile["General"]["ResolutionY"];
        const char* name = configFile["General"]["Name"];

        configFile["General"]["ResolutionX"] = 1280;
        configFile["General"]["ResolutionY"] = 720;
        configFile["General"]["String"] = "Hello World!";

        //configFile["Rendering"]["Alpha"] = false;
        //ConfigFile::Category& general = configFile["General"];

        //general["test"] = 52;

        //int val = general["test"];


        //VariantType t(20);
        //VariantType t2(3.14f);
        //VariantType t3(2.5353);
        //F32 test = t;
        //U64 test2 = t2;
        //F64 test3 = t3;
        //VariantType t4("Hello World");
        //const char* test4 = t4;

        int ehehe = 52;
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
