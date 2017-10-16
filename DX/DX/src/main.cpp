

#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/include.hpp"
#include "Core/subsystem_manager.h"

#include "Core/OS/FileSystem/file.h"

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

//ConfigFile("path.ini");
//ConfigFile["General"]["ResolutionX"] = 100;
//ConfigFile["General"]["ResolutionY"] = 100;
// <<< produces >>>>
// [General]
// ResolutionX = 100
// ResolutionY = 100

// Reading chars + numbers!

// TODO: Different amount of decimals

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
        OS::File file;
        file.open( "test.txt", false );

        //Thread& t = file.readAsync( []( String data ) {
        //    LOG( data );
        //});
        // t.wait();

        float i = 65.0f;
        float* p = &i;

        struct Vertex
        {
            float x;
            float y;
        };
        Vertex v{65,66};

        if ( file.exists() )
        {
            LOG( "File exists!\n" );

            file.write("A\n");
            file.write("B\n");
            file.write("C\n");
            file.write("[General]\n");
            file.write("Value=");
            file.write(66.0);
            file.write("\n");
            file.write(77);

            Size fileSize = file.getFileSize();
            LOG( fileSize );

            while (!file.eof())
            {
                U8 nextChar = file.readChar();
                String c(1, nextChar);
                LOG( c );
            }



        }
        else
        {
            LOG( "Creating new file...\n" );

            file.write( "TEST" );

            //String buffer = file.readAll();
            //LOG( buffer );
        }


    //    if ( file.exists() )
    //    {
    //        LOG( "File exists!\n" );

    //        // file.deleteFromDisk();
    //        file.clear();
    //        file.write("1\n");

    //        while (!file.eof())
    //        {
    //            char c = file.readChar();
    //            int i = 0;
    //        }

    //        LOG( "<<<<< FINAL FILE CONTENTS: >>>>> ", Color::RED);
    //        String buffer = file.readAll();
    //        LOG(buffer, Color::RED);

    //        Size size = file.getFileSize();
    //        LOG( "Size in bytes: " + TS(size) );

    //    }
    //    else
    //    {
    //        LOG( "Creating new file...\n" );

    //        file.write( "TEST" );

    //        String buffer = file.readAll();
    //        LOG( buffer );
    //    }
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
