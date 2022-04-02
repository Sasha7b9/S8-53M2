// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once 
#include "defines.h"


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

    bool GetNameForNewFile(char name[255]);

    void PressTab();
};
