// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"


int main()
{
    DEBUG_POINT_0;

    Device::Init();

    DEBUG_POINT_0;

    while(1)
    {
        Device::Update();
    }
}
