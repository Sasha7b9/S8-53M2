#pragma once
#include "Data/Data.h"


// �����������
namespace Averager
{
    // �������� ����� ������ ��� ����������
    void Append(DataSettings *ds);

    // �������� ���������� ������ �� ���������� ����������.
    void GetData(DataStruct &);

    // �������� ��������� �� ���������, ��������������� �������
    void Reset();
}

