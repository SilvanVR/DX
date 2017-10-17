

#include <Windows.h>
#undef ERROR

#include "Includes.hpp"
#include "TestClasses.hpp"
#include "MemoryManagement.hpp"
#include "FileStuff.hpp"


int main()
{
    SubSystemManager subSystemManager;
    subSystemManager.init();
    Locator::getLogger().setSaveToDisk(false);

    {
        //TestMemoryManagement();
        TestFileStuff();
    }


    subSystemManager.shutdown();
    system("pause");
    return 0;
}