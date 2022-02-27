// 2022/02/23 08:44:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"


namespace FPGA
{
    namespace Reader
    {
        extern Mutex mutex_read;                    // ������� �� ������ ������

        // ���������� �����, � �������� ����� ������ ������ �����
        uint16 CalculateAddressRead();

        // �������� ������ ����� ��� ������
        int DeltaRead();
    }

    namespace Launch
    {
        // ��������� � ���������� ����� ����- � �����- �������
        void Load();

        // ���������� �������� �����������, ������� ��� ������ � �������
        uint16 PredForWrite();
    }
}
