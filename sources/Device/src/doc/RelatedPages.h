//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \page page1 �������� ����
    \tableofcontents

    \section sec1 �������� �������� ����

    �������� �������� ���� ����� ��� Page. ��� ���:
    \code
    +------------+-------------+
    |  ��������  | ��������    |
    |  � ����    | ������ ���� |
    +------------+-------------+
    | pDisplay  | �������     |
    | pChanA    | ����� 1     |
    | pChanB    | ����� 2     |
    | pTrig     | �����       | 
    | pTime     | ���������   |
    | pCursors  | �������     | 
    | pMemory   | ������      |
    | pMeasures | ���������   |
    | pService  | ������      |
    | mpHelp     | ������      |
    +------------+-------------+
    \endcode

    \section sec2 �������� ������������ �������� ����
    \code
    +-------------+-------------+---------------------------------------------+
    | ��������� � | ���         | ����������                                  |
    | ��������    | ���������   |                                             |
    +-------------+-------------+---------------------------------------------+
    | mb          | Button      |                                             |
    | mc          | Choice      |                                             |
    | mp          | Page        | �������� ������� ������� ����               |
    | msp         | Page        | ��� ��������� �������� ���� - "�����������" |
    | sb          | SmallButton |                                             |
    +-------------+-------------+---------------------------------------------+
    \endcode
    ����� ��� �������� �������� ���������. ��������, \b mcShow - ��������� Choice, ������������� ����� ���� <b>������� - ����������</b>. 
    �� ����� "����������" ��������� Show, � "�������" ����� �� ���������� � ��������, ������ ��� �������� ��������� ������� ���������� ��������� 
    �������� ������.

    \section sec3 �������� ������������ ������ ����
    \code
    ������
    +-----------+-------------+-------------------------------------------------------------------------------+
    | ������    | � ����      | ��������                                                                      |
    |           | ��������    |                                                                               |
    +-----------+-------------+-------------------------------------------------------------------------------+
    | DrawSB    | SmallButton | ������� ���������� ��������� SmallButton                                      |
    | IsActive  | All         | ������� ���������� true, ���� ������� �������. ��������� �� ���� ���������    |
    | OnChanged | Choice      | ������� ���������� ��� ��������� ��������� Choice                             |
    | OnPress   | Button      | ������� �� ������� �������� ���� Button                                       |
    | OnRotate  | Regulator   | ������� �� ������� �����                                                      |
    +-----------+-------------+-------------------------------------------------------------------------------+
    \endcode
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \page page2 �������� ��������� ������

    �������� ����� �������� � OTP(One-Time-Programming (���������� ���������������)) ������.\n
    ��������� OTP-������ ������� � ������ 2.7 ��������� "PM0059 Programming manual STM32F205/215, STM32F207/217 Flash programming manual".\n
    � ������ �������� ����� � ��������� ���� �������� � 16 ������ �� ������ 0x1FFF7800.\n
    ������ �� �������� ����� ������������ �������� �������� ����� (��������, � ������ ��������� �������� �������). � ���� ������ 16 ����, � ������� 
    ������� ������� (�������� �������� �����), ����������� ������, � ����� ����� ������������ � ��������� 16 ������. �.�. �������� ���������� ��������� 
    ������ �����.\n
        1. ������������� ����� address = 0x1FFF7800.\n
        2. C�������� 16 ����, ������� � ������ address.\n
        3. ����������� address �� 16 ����.\n
        4. ���� ��� ��������� ����� ����� 0, �� ��������� �� 2.\n
        5. � ��������� ������ ��������� �������� ����� � ���� ��������� ������, ��������������� ����.\n
    �������� ����� ������������ �� ���� <b>�������</b> - <b>�/�</b>.
*/