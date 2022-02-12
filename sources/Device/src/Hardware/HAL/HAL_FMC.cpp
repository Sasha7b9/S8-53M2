// 2022/2/12 9:46:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


static const uint _ADDR_BANK = 0x60000000;
static const uint _ADDR_RAM = _ADDR_BANK + 0x04000000;

// ����� ������ � ���������� ��������. 0x100000 - ��� �������� ����� ��� ��������� A19 �� FPGA � 1. ���� �� �������
// ������ ���� 0x80000 (������ ��� a18 �������� �� 0x40000)
uint16 *const HAL_FMC::_ADDR_FPGA = (uint16 *)(_ADDR_BANK + 0x00000000 + 0x100000); //-V566

uint16 *const HAL_FMC::_ADDR_RAM_DISPLAY_FRONT = (uint16 *)(_ADDR_RAM + 1024 * 1024 - 320 * 240); //-V566
