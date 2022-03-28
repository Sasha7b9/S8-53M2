#pragma once
#include "Data/Data.h"


// �����������
namespace Averager
{
    // �������� ����� ������ ��� ����������
    void Append(const DataFrame &);

    // �������� ���������� ������ �� ���������� ����������.
    const DataStruct &GetData();
}


// ������ ���-����
namespace Limitator
{
    // ������� �������� ���, ���� � ����
    void ClearLimits();

    void Append(const DataStruct &);

    // �������� �������������� ����� ������� 0 - �����, 1 - ������.
    DataStruct &GetLimitation(Chan, int direction, DataStruct &);
}
