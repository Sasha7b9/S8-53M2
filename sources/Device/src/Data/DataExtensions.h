#pragma once
#include "Data/Data.h"


// �����������
namespace Averager
{
    // �������� ����� ������ ��� ����������
    void Append(const DataSettings *, uint8 *dataA, uint8 *dataB);

    // �������� ���������� ������ �� ���������� ����������.
    void GetData(DataStruct &);

    void Reset(DataStruct &);
}

