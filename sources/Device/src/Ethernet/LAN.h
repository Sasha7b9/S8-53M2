// 2022/02/11 17:48:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class LAN
{
public:

    static void Init();
    // ������� ����� ������������ �������/�������� ����� timeMS �����������. ��� ���� ��� ������� ������� �������� ����������. WARN ��������. ���� ���������.
    static void Update(uint timeMS);
};
