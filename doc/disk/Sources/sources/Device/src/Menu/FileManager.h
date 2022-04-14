// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"
#include "Utils/Text/String.h"


// ���������� ���� ��� ��� ����������� ����� ������.
namespace FM
{
    extern int needRedraw;      // ���� 1, �� ����-�������� ��������� � ������ �����������
                                // ���� 2, �� ������������ ������ ��������
                                // ���� 3, �� ������������ ������ �����

    extern bool needOpen;       // ���� 1, �� ����� ������� �������� �������� (��������� ���������������)

    void Init();

    void Draw();

    void PressLevelUp();

    void PressLevelDown();

    void RotateRegSet(int angle);

    String<> GetNameForNewFile();

    void PressTab();
};
