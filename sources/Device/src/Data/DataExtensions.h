#pragma once
#include "Data/Data.h"


// �����������
namespace Averager
{
    // �������� ����� ������ ��� ����������
    void Append(DataStruct &);

    // �������� ���������� ������ �� ���������� ����������.
    void GetData(DataStruct &);

    // �������� ��������� �� ���������, ��������������� �������
    void Reset();
}

