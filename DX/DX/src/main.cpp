

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

// Read until specific character has been reached
// jump to line
// different modes

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

        if ( file.exists() )
        {
            LOG( "File exists!\n" );

            file.clear();

            file.write(152);
            file.write(" ");
            file.write(3.14f);
            file.write(" ");
            file.write(17.14353);

            U32 n1 = file.nextU32();
            F32 n2 = file.nextF32();
            F64 n3 = file.nextF64();

            if (!file.eof())
            {
                F64 n4 = file.nextChar();

            }

            //file.write("%s= %d\n", "Value", 42);
            //file.write("%s= %d\n", "Value2", 13);

            //char str[10];
            //int number;
            //file.read("%s %d", str, &number);

            //char str2[10];
            //int number2;
            //file.read("%s %d", str2, &number2);

            //file.setReadCursor(2);
            //double num = file.nextDouble();
            //LOG( TS(num) );

            //file.write("A\n");
            //file.write("B\n");
            //file.write("C\n");
            //file.write("[General]\n");
            //file.write("Value=");
            //file.write(66);

            //file.seekWriteCursor(0);
            //file.append("Z");

            LOG("----- Listing File-Contents: ----- ", Color::YELLOW);
            String fileContents = file.readAll();
            LOG(fileContents, Color::YELLOW);
            LOG("---------------------------------- ", Color::YELLOW);
            //while (!file.eof())
            //{
            //    String line = file.readLine();
            //    LOG(line);
            //}

            Size fileSize = file.getFileSize();
            LOG( "Bytes: " + TS(fileSize) );
      /*      while (!file.eof())
            {
                U8 nextChar = file.readChar();
                String c(1, nextChar);
                LOG( c );
            }*/

        }
        else
        {
            LOG( "Creating new file...\n" );

            file.write("");
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
