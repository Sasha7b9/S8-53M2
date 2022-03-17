// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace VCP
{
    void Init();

    void SendBuffer(const uint8 *buffer, int size);

    void SendSynch(const uint8 *buffer, int size);
    
    void SendAsynch(char *format, ...);

    // ��������� ��������� ��� ������������ ����
    void SendMessage(pchar message);

    void Flush();

    extern bool cableIsConnected;       // true, ���� ���������� ������
    extern bool connectToHost;          // true, ���� ���� �����������
};
