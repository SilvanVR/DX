#pragma once


MemoryManagement::UniversalAllocator gUniversalAllocator(1000);


void TestMemoryManagement()
{
    const int SIZE = 1000;

    LOG("MEASURE NEW + DELETE...");
    static A* a[SIZE];
    {
        AutoClock clock;
        
        for (int i = 0; i < SIZE; i++)
        {
            a[i] = new A();
        }
        for (int i = 0; i < SIZE; i++)
        {
            delete a[i];
        }
    }

    {
        LOG("MEASURE POOL ALLOCATOR...");
        static A* a2[SIZE];
        MemoryManagement::PoolAllocator poolAllocator(sizeof(A), SIZE);
        {
            AutoClock clock;

            for (int i = 0; i < SIZE; i++)
            {
                a2[i] = poolAllocator.allocate<A>();
            }
            for (int i = 0; i < SIZE; i++)
            {
                poolAllocator.deallocate(a2[i]);
            }
        }
    }


    {
        LOG("MEASURE STACK ALLOCATOR...");
        static A* a3[SIZE];
        MemoryManagement::StackAllocator stackAllocator(SIZE * sizeof(A) * 2);
        {
            AutoClock clock;
            for (int i = 0; i < SIZE; i++)
            {
                a3[i] = stackAllocator.allocate<A>();
            }
            stackAllocator.clear();
        }
    }

    {
        MemoryManagement::PoolListAllocator poolListAllocator({ 8, 16, 32, 64, 128, 256 }, 32);

        A* a;
        a = poolListAllocator.allocate<A>();
        poolListAllocator.deallocate(a);

        Size* f = poolListAllocator.allocate<Size>();
        poolListAllocator.deallocate(f);

        void* test = poolListAllocator.allocateRaw(255);
        poolListAllocator.deallocate(test);
    }

    {
        MemoryManagement::UniversalAllocatorDefragmented universalDefragmentedAllocator(1000, 10);

        auto a = universalDefragmentedAllocator.allocate<A>(1);
        A* aRaw = a.getRaw();
        universalDefragmentedAllocator.deallocate(a);

        MemoryManagement::UAPtr<B> b = universalDefragmentedAllocator.allocate<B>(1);
        MemoryManagement::UAPtr<A> a2 = b;
        universalDefragmentedAllocator.deallocate(a2);

        auto raw = universalDefragmentedAllocator.allocateRaw(1);
        Byte* r = raw.getRaw();
        universalDefragmentedAllocator.deallocate(raw);

        MemoryManagement::UAPtr<U32> u32 = universalDefragmentedAllocator.allocate<U32>();
        //MemoryManagement::UAPtr<Byte> test = u32;
        universalDefragmentedAllocator.deallocate(u32);

        MemoryManagement::UAPtr<U32> arr[10];
        for (int i = 0; i < 10; i++)
        {
            arr[i] = universalDefragmentedAllocator.allocate<U32>(1);
            *arr[i] = i;
        }
        for (int i = 0; i < 10; i++)
        {
            LOG(*arr[i]);
            universalDefragmentedAllocator.deallocate(arr[i]);
        }

        auto a5 = universalDefragmentedAllocator.allocate<A>();
        auto a6 = universalDefragmentedAllocator.allocate<A>();

        universalDefragmentedAllocator.deallocate(a5);

        a6->time = OS::PlatformTimer::getCurrentTime();
        //LOG( a6->time.toString() );
        //LOG( a6.getRaw() );

        // A6 should be shifted
        universalDefragmentedAllocator.defragmentOnce();

        //LOG( a6->time.toString() );
        //LOG( a6.getRaw() );

        universalDefragmentedAllocator.deallocate(a6);
    }


}
