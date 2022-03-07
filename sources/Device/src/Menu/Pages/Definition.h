// 2022/2/11 19:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsCursors.h"


class Page;


struct PageChannelA
{
    static void OnChanged_Input(bool active);
};


struct PageChannelB
{
    static void OnChanged_Input(bool active);
};


struct PageCursors
{
    // Получить позицию курсора напряжения.
    static float GetCursPosU(Chan::E, int numCur);

    // Возвращает true,если нужно рисовать курсоры.
    static bool NecessaryDrawCursors();

    // Получить строку курсора напряжения.
    static pchar GetCursVoltage(Chan::E source, int numCur, char buffer[20]);

    // Получить строку курсора времени.
    static pchar GetCursorTime(Chan::E source, int numCur, char buffer[20]);

    // Получить строку процентов курсоров напряжения.
    static pchar GetCursorPercentsU(Chan::E source, char buffer[20]);

    // Получить строку процентов курсоров времени.
    static pchar GetCursorPercentsT(Chan::E source, char buffer[20]);

    static void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);

    static void Cursors_Update();    // Вызываем эту функцию для каждого измерения, чтобы обновить положие курсоров, если они должны обновляться автоматически.

    struct PageSet
    {
        static void *pointer;
    };

    static void *GetPointer();
};


struct PageDebug
{
    struct SerialNumber
    {
        static Page *GetPointer();
    };
};


struct PageHelp
{
    static void *GetPointer();
};


struct PageMeasures
{
    struct Tune
    {
        static void *pointer;
    };

    // Если true, то активен выбор типа измерений для выбора на странице ИЗМЕРЕНИЯ-НАСТРОИТЬ
    static bool choiceMeasuresIsActive;
};


struct PageMemory
{
    static void OnChanged_NumPoints(bool active);
    static void OnPressExtFileManager();
    static void SaveSignalToFlashDrive();
    static const void *pMspFileManager;

    struct Latest
    {
        static int16 current;               // Текущий номер последнего сигнала в режиме ПАМЯТЬ - Последние

        static const Page *GetPointer();
    };

    struct Internal
    {
        static int8 currentSignal;          // Текущий номер сигнала, сохранённого в ППЗУ

        static bool showAlways;             // Если 1, то показывать всегда выбранный в режиме "Внутр. ЗУ" сигнал

        static Page *self;
    };

    struct SetMask
    {
        static const Page *GetPointer();
    };

    struct SetName
    {
        static Page *self;
    };

    struct FileManager
    {
        static const Page *GetPointer();
    };

    static void *GetPointer();
};


struct PageService
{
    struct Math
    {
        static bool Enabled();

        struct Function
        {
            static void *GetPointer();
        };

        struct FFT
        {
            struct Cursors
            {
                static void *GetPointer();
            };
        };
    };

    struct Information
    {
        static const Page *GetPointer();
    };
};


#define COMMON_BEGIN_SB_EXIT  0, "Выход", "Exit", "Кнопка для выхода в предыдущее меню", "Button for return to the previous menu"


bool IsMainPage(void *item);            // Возвращает true, если item - адрес главной страницы меню.


void CalculateConditions(int16 pos0, int16 pos1, CursCntrl, bool *cond0, bool *cond1);    // Рассчитывает условия отрисовки УГО малых кнопок управления выбором курсорами.
void DrawMenuCursTime(int x, int y, bool left, bool right);
void DrawSB_Exit(int x, int y);
