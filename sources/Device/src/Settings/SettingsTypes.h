// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// SDisplay


// Режим отображения пропущенных сигналов при ограничении частоты кадров.
enum MissedSignals
{
    Missed_Hide,    // Не выводить на экран
    Missed_Show,    // Выводить на экран
    Missed_Average  // Устреднять и выводить на экран
};



// ChannelX
struct Chan 
{ 
    enum E
    {
        A,
        B,
        A_B,
        Math,
        Count = 2
    };

    E value;

    Chan(E v = A) : value(v) {}

    bool Enabled();

    // Возвращает номер канала от 1 до 2
    int ToNumber() { return (value == Chan::A) ? 1 : 2; }

    bool IsA() const { return value == A; }
    bool IsB() const { return value == B; }

    operator int() { return (int)value; }
};


#define ChA Chan::A
#define ChB Chan::B


// Делитель.
struct Divider
{
    enum E
    {
        _1,
        _10
    };

    static int ToAbs(Divider::E);
};

// Measures


// Сжимать ли сигналы при выводе измерений.
enum ModeViewSignals
{
    ModeViewSignals_AsIs,       // Показывать сигналы как есть
    ModeViewSignals_Compress    // Сжимать сетку с сигналами
};


// Math


enum ScaleMath
{
    ScaleMath_Channel0,     // Масштаб берётся из канала 1
    ScaleMath_Channel1,     // Масштаб берётся из канала 2
    ScaleMath_Hand          // Масштаб задаётся вручную.
};


// Курсоры

// Каким курсором управлять.
struct CursCntrl
{
    enum E
    {
        _1,         // первым.
        _2,         // вторым.
        _1_2,       // обоими.
        Disable     // никаким.
    };
};


// Дискретность перемещения курсоров.
struct CursMovement
{
    enum E
    {
        Points,    // по точкам.
        Percents   // по процентам.
    };
};

// Какие курсоры сейчас активны. Какие активны, те и будут перемещаться по вращению ручки УСТАНОВКА.
struct CursActive
{
    enum E
    {
        U,
        T,
        None
    };
};

// Режим слежения курсоров.
enum CursLookMode
{
    CursLookMode_None,      // Курсоры не следят.
    CursLookMode_Voltage,   // Курсоры следят за напряжением автоматически.
    CursLookMode_Time,      // Курсоры следят за временем автоматически.
    CursLookMode_Both       // Курсоры следят за временем и напряжением, в зависимости от того, какой курсоры вращали последним.
};


// Service


// Язык меню
enum Language
{
    Russian,    // Русский
    English     // Английский
};


// Тип дополнительного смещения
enum RShiftADCtype
{
    RShiftADC_Disable,
    RShiftADC_Settings,
    RShiftADC_Hand
};
