// 2022/02/11 17:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


namespace Display
{
    const int WIDTH = 320;
    const int HEIGHT = 240;

    static const int DELTA = 5;

    extern uint8 *const _back_buffer;

    extern bool framesElapsed;
    extern int  numDrawingSignals;  // Число нарисованных сигналов для режима накопления

    extern int topMeasures;         // Верх таблицы вывода измерений. Это значение нужно для нормального вывода сообщений на экран - чтобы 
                                    // они ничего не перекрывали

    void Init();

    void Update(bool endScene = true);

    void RotateRShift(Chan);

    void RotateTrigLev();

    void EnableTrigLabel(bool enable);

    void SetDrawMode(DrawMode::E, pFuncVV = nullptr);

    void SetAddDrawFunction(pFuncVV func);

    void RemoveAddDrawFunction();

    void Clear();

    void ShiftScreen(int delta);

    // После отрисовки очередного экрана эта функция будет вызвана один раз.
    void RunAfterDraw(pFuncVV func);
};


// Display Hardware
namespace DH
{
    class HLine
    {
    public:
        HLine(int y);
        ~HLine();
        void Fill(int x1, int x2, uint8);
    private:
        int y;
        uint8 line[320];
    };

    class VLine
    {
    public:
        VLine(int x);
        ~VLine();
        void Fill(int y1, int y2, uint8);
    private:
        int x;
        // Т.к. из памяти можно читать только по 16 бит за раз, сюда читаются сразу два столбика - чётный и нечётный
        // Если x - чётное, то работаем с младшим байтом, если нечётное - со старшим байтом
        uint16 line[240];
    };

    class Point
    {
    public:
        Point(int x, int y);
        ~Point();
        void Fill(uint8);
        uint8 Get();
    private:
        int x;
        int y;
    };
}
