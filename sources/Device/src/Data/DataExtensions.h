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

    void CalculateLimits(const DataSettings *, const uint8 *dataA, const uint8 *dataB);

    // �������� �������������� ����� ������� 0 - �����, 1 - ������.
    DataStruct &GetLimitation(Chan, int direction, DataStruct &);
}
