// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// ������������ ��� ���������� �������� �������
namespace HAL_DAC1
{
    void Init();

    // �������� value �� 0 �� 100
    void SetBrightness(int value);
};


// ������������ ��� ����� � FPGA � RAM
namespace HAL_FMC
{
    extern uint16 *const ADDR_RAM_DISPLAY_FRONT;    // ������� ����������� �������. ��������� � RAM � �� ����� ������-
                                                    // �������� ��� ������ �����
}


namespace HAL_LTDC
{
    void Init(uint8 *frontBuffer, uint8 *backBuffer);

    void LoadPalette();

    void ToggleBuffers();
};
