#include "Includes.hpp"
#include "TestClasses.hpp"
#include "MemoryManagement.hpp"
#include "FileStuff.hpp"
#include "Threading.hpp"

#include "Common/enum_class_operators.hpp"

//----------------------------------------------------------------------
enum class CameraFlags
{
    None = 0,
    BlitToScreen = 1 << 0,
    BlitToLeftEye = 1 << 1,
    BlitToRightEye = 1 << 2
};
ENABLE_BITMASK_OPERATORS(CameraFlags)

int main()
{
    CameraFlags flags = CameraFlags::None | CameraFlags::BlitToScreen;

    //constexpr CameraFlags flags2 = CameraFlags::None & CameraFlags::BlitToScreen;

    if ((flags & CameraFlags::BlitToScreen) != CameraFlags::None)
    {

        I32 i = 52;
    }

    system("pause");
    return 0;
}