// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Display.h"
#include "Display/Colors.h"
#include "defines.h"

                                            
#define MAX_NUM_SUBITEMS_IN_CHOICE  12  // ������������ ���������� ��������� ������ � �������� Choice.
#define MAX_NUM_ITEMS_IN_PAGE       15  // ������������ ���������� ������� �� ��������.
#define MENU_ITEMS_ON_DISPLAY       5   // ������� ������� ���� ���������� �� ������ �� ���������.
#define MAX_NUM_CHOICE_SMALL_BUTTON 6   // ������������ ���������� ��������� ��������� ������ + 1


// ������ ���� ������� ����
struct TypeItem
{
    enum E
    {
        None,
        Choice,        // ����� ������ - ��������� ������� ���� �� ���������� �������� ��������.
        Button,        // ������.
        Page,          // ��������.
        Governor,      // ��������� - ��������� ������� ����� ����� �������� �������� �� ������� ��������� ����������.
        Time,          // ��������� ������ �����.
        IP,            // ��������� ������ IP-�����.
        SwitchButton,
        GovernorColor, // ��������� ������� ����.
        Formula,       // ��������� ������� ���� � ������������ ��� �������������� ������� (��������� � ��������)
        MAC,           // MAC-�����
        ChoiceReg,     // ������� ������, � ������� ����� �������������� �� �������, � ������
        SmallButton,   // ������ ��� ������ ����� ������
        Count
    };
};

// ����� ID ���� �������������� � ���� �������
struct NamePage
{
    enum E
    {
        Display,               // ������
        DisplayAccumulation,   // ������� - ����������
        DisplayAverage,        // ������� - ����������
        DisplayGrid,           // ������� - �����
        DisplayRefresh,        // ������� - ����������
        MainPage,              // ������� �������� ����. �� �� ������ ��� ���������
        Debug,                 // �������
        DebugConsole,          // ������� - �������
        DebugADC,              // ������� - ���
        DebugADCbalance,       // ������� - ��� - ������
        DebugADCstretch,       // ������� - ��� - ��������
        DebugADCshift,
        DebugADCrShift,        // ������� - ��� - ���. ��������
        DebugADCaltShift,      // ������� - ��� - ���. ����. ���.
        DebugShowRegisters,    // ������� - ��������
        DebugRandomizer,       // ������� - ����-���
        DebugInformation,      // ������� - ����������
        DebugSound,            // ������� - ����
        DebugAltera,           // ������� - �������
        Channel0,              // ����� 1
        Channel1,              // ����� 2
        Cursors,               // �������
        Trig,                  // �����.
        Memory,                // ������
        MemoryExt,             // ������ - �����. ��
        Measures,              // ���������
        MeasuresField,         // ��������� - ����
        Service,               // ������
        ServiceDisplay,        // ������ - �������
        ServiceDisplayColors,  // ������ - ������� - �����
        ServiceCalibrator,     // ������ - ����������
        ServiceEthernet,       // ������ - ETHERNET
        Help,                  // ������
        Time,                  // ���������
        Math,                  // ������ - ����������
        MathFFT,
        TrigAuto,              // ����� - ��� �����
        SB_Curs,               // ������� �������� ������� - ���������
        SB_MeasTuneMeas,       // ������� �������� ��������� - ���������
        SB_MeasTuneField,      // ������� �������� ��������� - ���� - ������
        SB_MemLatest,          // ������� �������� ������ - ���������
        SB_MemInt,             // ������� �������� ������ - ����� ��
        SB_MemExtSetMask,      // ������� �������� ������ - ����� �� - �����
        SB_MemExtSetName,      // ���������� ����� ����������� ����� �� ������ ��� ������� ����� ����� ��� ��������������� ����� ����� �� - ��� �����
        SB_FileManager,        // ������� �������� ������ - ����� �� - �����
        SB_Help,               // ������� �������� HELP
        SB_MathCursorsFFT,     // ������� �������� ������ - ���������� - �������
        SB_MathFunction,       // ������ - ���������� - �������
        SB_Information,        // ������ - ����������
        SB_SerialNumber,       // ������ - �/�
        Count,
        NoPage
    };
};             // ��� ���������� ������� ����� �������� ������������� SettingsMenu � SSettings.c

class Page;
class SmallButton;

#define COMMON_PART_MENU_ITEM                                                                                     \
    TypeItem::E         type;             /* ��� ����� */                                                     \
    const class Page*   keeper;           /* ����� ��������, ������� �����������. ��� NamePage::MainPage = 0 */    \
    pFuncBV             funcOfActive;     /* ������� �� ������ ������� */                                     \
    pchar               titleHint[4];     /* �������� �������� �� ������� � ���������� ������. ����� ��������� ��� ������ ������ */



struct ItemStruct
{
    COMMON_PART_MENU_ITEM
};


class Item
{
public:
    TypeItem::E       type;         /* ��� ����� */
    const class Page *keeper;       /* ����� ��������, ������� �����������. ��� NamePage::MainPage = 0 */
    pFuncBV           funcOfActive; /* ������� �� ������ ������� */
    pchar             titleHint[4]; /* �������� �������� �� ������� � ���������� ������. ����� ��������� ��� ������ ������ */

public:
    Item(const ItemStruct *str);
    Item(TypeItem::E, const Page* keeper_, pFuncBV funcOfActive_, pchar tileRU, pchar titleEN, pchar hintRU, pchar hintEN);
    // ���������� true, ���� ������, ��������������� ������� �������� ����, ��������� � ������� ���������.
    bool IsPressed();

    // ���������� ����� ��������� �������� ����.
    static Item *Opened();

    // ���������� ����� ������ ����, ������������ ��� ������� � ������ ������ �������.
    static void *UnderKey();

    // ���������� �������� �������� �� ������ item, ��� ��� �������� �� ������� �������.
    pchar Title();
};


#define TITLE(item) ((item)->titleHint[LANG])
#define HINT(item) ((item)->titleHint[2 + LANG])

typedef void * pVOID;
typedef pVOID arrayItems[MAX_NUM_ITEMS_IN_PAGE];


// ��������� �������� ����.
class Page : public Item
{
public:
    NamePage::E       name;         // ��� �� ������������ NamePage
    const arrayItems *items;        // ����� ��������� �� ������ ���� �������� (� ������� ��������)
                                    // ��� �������� ����� ������  ����� �������� 6 ���������� �� SmallButton :
                                    // 0 - Key::Menu, 1...5 - Key::F1...Key::F5
    pFuncVV           funcOnPress;  // ����� ���������� ��� ������� �� �������� ��������
    pFuncVV           funcOnDraw;   // ����� ���������� ����� ��������� ������
    pFuncVI           funcRegSetSB; // � �������� ����� ������ ���������� ��� �������� ����� ���������

    Page(const Page *keeper_, pFuncBV funcOfActive_,
         pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, NamePage::E name_,
         const arrayItems *items_, pFuncVV funcOnPress_ = 0, pFuncVV funcOnDraw_ = 0, pFuncVI funcRegSetSB_ = 0);

    void DrawOpened(int layer, int yTop);

    void Draw(int x, int y);

    void DrawTitle(int layer, int yTop);

    // ���������� ������� �������� �������� ������� page.
    int8 PosCurrentItem() const;

    void DrawItems(int layer, int yTop);

    void DrawUGO(int right, int bottom);

    bool IsSB() const;

    // ���������� ��� �������� page.
    NamePage::E GetName() const;

    // ���������� ������� ������� �������� �������� �� ������ page �� ������. ���� ������� ����������� 0, ��� ����� 0,
    // ���� ������� ����������� 1, ��� ����� 5 � �.�.
    int PosItemOnTop();

    // ���������� ����� ������� ���������� �������� �� ������ page.
    int NumCurrentSubPage() const;

    // ���������� ���������� ��������� � �������� �� ������ page.
    int NumItems() const;

    // D��������� ����� ���������� � �������� �� ������ page.
    int NumSubPages() const;

    // ���������� ����� �������� ���� �������� ��������.
    void *GetItem(int numElement) const;

    // ���������� true, ���� ������� ������� �������� � ������ namePage ������.
    bool CurrentItemIsOpened();

    void ChangeSubPage(int delta) const;

    SmallButton *GetSmallButon(int numButton);

    void OpenAndSetCurrent() const;

    // ���������� �������� ����, ������� ������ ����������� �� ������� ������
    static const Page *ForButton(Key::E);

    static Page *FromName(NamePage::E);

    // ������������� ����� ������� ����������� � �������� namePage.
    void SetCurrentSubPage(int8 posSubPage) const;
};


// ��������� ������.
class Button : public Item
{
public:
    pFuncVV         funcOnPress;    // �������, ������� ���������� ��� ������� �� ������.

    Button(const Page *keeper_, pFuncBV funcOfActive_,
            pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, pFuncVV funcOnPress_);

    void Draw(int x, int y);
};


class StructHelpSmallButton
{
public:
    pFuncVII    funcDrawUGO;    // ��������� �� ������� ��������� ����������� �������� ������
    pchar helpUGO[2];     // ������� � ������� �����������.
};

typedef StructHelpSmallButton arrayHints[MAX_NUM_CHOICE_SMALL_BUTTON];


// ��������� ������ ��� ��������������� ������ ����.
class SmallButton : public Item
{
public:
    pFuncVV             funcOnPress;    // ��� ������� ��������� ��� ��������� ������� ������. ���������� true, ���� ���� ��
    pFuncVII            funcOnDraw;     // ��� ������� ���������� ��� ��������� ������ � ����� � ������������ x, y.
    const arrayHints   *hintUGO;

    SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
                pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
                pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_ = 0);

    void Draw(int x, int y);
};


// ��������� ���������.
class Governor : public Item
{ 
public:
    int16*      cell;
    int         minValue;               // ���������� ��������, ������� ����� ��������� ���������.
    int16       maxValue;               // ������������ ��������.
    pFuncVV     funcOfChanged;          // �������, ������� ����� �������� ����� ����, ��� �������� ���������� ����������.
    pFuncVV     funcBeforeDraw;         // �������, ������� ���������� ����� ����������

    Governor(const Page *keeper_, pFuncBV funcOfActive_,
             pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
             int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_ = 0, pFuncVV funcBeforeDraw_ = 0);

    void Draw(int x, int y, bool opened);

    // ��������� �������� �������� ���������� ��� ���������� �������� ���� ���� Governor (� ����������� �� ����� delta)
    void StartChange(int detla);

    // ������������ ��������� ���� ��������.
    float Step();

    // �������� �������� � ������� ������� ��� ��������� ��������
    void ChangeValue(int delta);

    // ���������� ��������� ������� ��������, ������� ����� ������� governor.
    int16 NextValue() const;

    // ���������� ��������� ������� ��������, ������� ����� ������� governor.
    int16 PrevValue() const;

    // ��� �������� �������� ������������ ������ �� ��������� �������
    void NextPosition();

    // ���������� ����� ��������� � ���� ��� ����� �������� governor. ������� �� ������������� ��������, ������� ����� ��������� governor.
    int NumDigits() const;

    void DrawOpened(int x, int y);

    void DrawLowPart(int x, int y, bool pressed, bool shade);

    void DrawClosed(int x, int y);

    static int8 cur_digit;
};


class Choice
{
public:
    COMMON_PART_MENU_ITEM
        pchar names[MAX_NUM_SUBITEMS_IN_CHOICE][2];   // �������� ������ �� ������� � ���������� ������.
    int8 *cell;                                   // ����� ������, � ������� �������� ������� �������� ������.
    pFuncVB	    funcOnChanged;                          // ������� ������ ���������� ����� ��������� �������� ��������.
    pFuncVII    funcForDraw;                            // ������� ���������� ����� ��������� ��������. 
    pchar NameSubItem(int i);
    // ���������� ��� �������� �������� ������ �������� choice, ��� ��� �������� � �������� ���� ���������.
    pchar NameCurrentSubItem();
    // ���������� ��� ���������� �������� ������ �������� choice, ��� ��� �������� � �������� ���� ���������.
    pchar NameNextSubItem();

    pchar NamePrevSubItem();
    // ���������� ���������� ��������� ������ � �������� �� ������ choice.
    int NumSubItems();

    void StartChange(int delta);
    // ������������ ��������� ���� ��������.
    float Step();
    // �������� �������� choice � ����������� �� �������� � ����� delta.
    void ChangeValue(int delta);

    void FuncOnChanged(bool active);

    void FuncForDraw(int x, int y);

    void Draw(int x, int y, bool opened);

    void DrawClosed(int x, int y);

    void DrawOpened(int x, int y);
};


struct IPaddressStruct
{
    ItemStruct str;
    uint8* ip0;
    uint8* ip1;
    uint8* ip2;
    uint8* ip3;
    pFuncVB funcOfChanged;
    uint16 *port;
};


class IPaddress : public Item
{
public:
    uint8*  ip0;
    uint8*  ip1;
    uint8*  ip2;
    uint8*  ip3;
    pFuncVB funcOfChanged;
    uint16* port;

    IPaddress(const IPaddressStruct *str);

    void Draw(int x, int y, bool opened);

    // ��� �������� �������� ������������ ������ �� ��������� �������
    void NextPosition() const;

    // �������� �������� � ������� ������� ��� �������� ��������
    void ChangeValue(int delta);

    // ���������� ����� �������� ����� (4 - ����� �����) � ����� ������� ������� � �����.
    void GetNumPosIPvalue(int *numIP, int *selPos);

    void DrawOpened(int x, int y);

    void DrawLowPart(int x, int y, bool pressed, bool shade);

    static int8 cur_digit;
};


class  MACaddress
{
public:
    COMMON_PART_MENU_ITEM
    uint8*  mac0;
    uint8*  mac1;
    uint8*  mac2;
    uint8*  mac3;
    uint8*  mac4;
    uint8*  mac5;
    pFuncVB funcOfChanged;

    void Draw(int x, int y, bool opened);

    void ChangeValue(int delta);

    void DrawOpened(int x, int y);

    void DrawValue(int x, int y);

    static int8 cur_digit;
};


// ��������� ������� ���� ��� ��������� ������������� � ������ �������������� �������
#define FIELD_SIGN_MEMBER_1_ADD 0
#define FIELD_SIGN_MEMBER_1_MUL 1
#define FIELD_SIGN_MEMBER_2_ADD 2
#define FIELD_SIGN_MEMBER_2_MUL 3
#define POS_SIGN_MEMBER_1       0
#define POS_KOEFF_MEMBER_1      1
#define POS_SIGN_MEMBER_2       2
#define POS_KOEFF_MEMBER_2      3


class Formula
{
public:
    COMMON_PART_MENU_ITEM
    int8*           function;                   // ����� ������, ��� �������� Function, �� ������� ������ ���� ��������
    int8*           koeff1add;                  // ����� ������������ ��� ������ ����� ��� ��������
    int8*           koeff2add;                  // ����� ������������ ��� ������ ����� ��� ��������
    int8*           koeff1mul;                  // ����� ������������ ��� ������ ����� ��� ���������
    int8*           koeff2mul;                  // ����� ������������ ��� ������ ����� ��� ���������
    int8*           curDigit;                   // ������� ������ : 0 - ���� ������� �����, 1 - ����������� ������� �����,
                                                //  2 - ���� ������� �����, 3 - ����������� ������� �����
    pFuncVV         funcOfChanged;              // ��� ������� ���������� ����� ��������� ��������� �������� ����������.

    void Draw(int x, int y, bool opened);

    void WriteText(int x, int y, bool opened);

    void DrawLowPart(int x, int y, bool pressed, bool shade);
};


class  GovernorColor
{
public:
    COMMON_PART_MENU_ITEM
    ColorType*      colorType;                  // ��������� ��� �������� �����.

    void Draw(int x, int y, bool opened);

    // �������� ������� ����� � governor
    void ChangeValue(int delta);

    void DrawOpened(int x, int y);

    void DrawValue(int x, int y, int delta);

    void DrawClosed(int x, int y);
};


#define iEXIT   0
#define iDAY    1
#define iMONTH  2
#define iYEAR   3
#define iHOURS  4
#define iMIN    5
#define iSEC    6
#define iSET    7

// ������������� � ���������� �����.
struct Time
{
    COMMON_PART_MENU_ITEM
    int8*       curField;       // ������� ���� ���������. 0 - �����, 1 - ���, 2 - ���, 3 - ����, 4 - ����, 5 - �����, 6 - ���, 7 - ����������.
    int8*       hours;
    int8*       minutes;
    int8*       seconds;
    int8*       month;
    int8*       day;
    int8*       year;

    void Draw(int x, int y, bool opened);

    void SetOpened();

    void SetNewTime() const;

    void SelectNextPosition();

    void IncCurrentPosition() const;

    void DecCurrentPosition() const;

    void DrawClosed(int x, int y);

    void DrawOpened(int x, int y);
};
