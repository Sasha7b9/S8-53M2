// 2022/02/23 08:44:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"


struct StateCalibration
{
    enum E
    {
        None,
        RShift0start,
        RShift0inProgress,
        RShift1start,
        RShift1inProgress,
        ErrorCalibration0,
        ErrorCalibration1
    };
};

struct StateWorkFPGA
{
    enum E
    {
        Stop,    // ���� - �� ���������� ����������� ����������.
        Wait,    // ��� ����������� ��������������.
        Work,    // ��� ������.
        Pause    // ��� ���������, ����� �������� ������������� ������, ��������, ��� ������ ������ ��� ��� 
                 // ������ �������� ���������.
    };

    E value;

    StateWorkFPGA(E v = Stop) : value(v) {}

    static StateWorkFPGA::E GetCurrent() { return current; }
    static void SetCurrent(StateWorkFPGA::E v) { current = v; }

private:

    static E current;
};


struct StateFPGA
{
    bool                needCalibration;               // ������������� � true �������� ��������, ��� ���������� ���������� ����������.
    StateWorkFPGA       stateWorkBeforeCalibration;
    StateCalibration::E stateCalibration;              // ������� ��������� ����������. ������������ � �������� ����������.
};


namespace FPGA
{
    namespace Reader
    {
        extern Mutex mutex_read;                    // ������� �� ������ ������

        // ��� ������� ������ ���������� ������ ��� ������ ������� ������. ��� ������ ������� ������ ����� ������������
        // ����� ���������� ��������
        // ������������ �������� �� 1 ������ ���������. �.�. ������ ������ ����� ��������� - ���-�� � ��������
        uint16 CalculateAddressRead();

        // ������ ������ ����� �� first �� last
        void ReadPoints(Chan, uint8 *first, const uint8 *last);

        void ReadPoints(Chan);
    }

    namespace Launch
    {
        // ��������� � ���������� ����� ����- � �����- �������
        void Load();

        // ���������� �������� �����������, ������� ��� ������ � �������
        uint16 PredForWrite();
    }
}
