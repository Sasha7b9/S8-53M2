// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"


int main()
{
    Device::Init();

    while(1)
    {
        Device::Update();
    }
}
