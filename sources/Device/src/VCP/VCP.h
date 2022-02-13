#pragma once


namespace VCP
{
    void Init();
    void SendDataSynch(const uint8 *data, int size);
    void SendStringSynch(char *data);                   // ��������� ������ ��� ������������ ����.
    void SendFormatStringAsynch(char *format, ...);     // ��� ������ ��������� � ������������ ��������� \r\n.
    void SendFormatStringSynch(char *format, ...);      // ��� ������ ��������� � ������������ ��������� \r\n.
    void Flush();
    void Update();

    extern void *handlePCD;
    extern void *handleUSBD;
};
