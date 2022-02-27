// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Идентификаторы кнопок.
struct Key
{
    enum E
    {
        Empty = 0,        // кнопка не нажата
        ChannelA = 1,        // КАНАЛ 1
        Service = 2,        // СЕРВИС
        ChannelB = 3,        // КАНАЛ 2
        Display = 4,        // ДИСПЛЕЙ
        Time = 5,        // РАЗВ
        Memory = 6,        // ПАМЯТЬ
        Trig = 7,        // СИНХР
        Start = 8,        // ПУСК/СТАРТ
        Cursors = 9,        // КУРСОРЫ
        Measures = 10,       // ИЗМЕР
        Power = 11,       // ПИТАНИЕ
        Help = 12,       // ПОМОЩЬ
        Menu = 13,       // МЕНЮ
        F1 = 14,       // 1
        F2 = 15,       // 2
        F3 = 16,       // 3
        F4 = 17,       // 4
        F5 = 18,       // 5
        Count            // общее количество кнопок
    };
};

Key::E& operator++(Key::E &);

// Идентификаторы регуляторов.
enum Regulator
{
    R_Empty     = 0,    // регулятор не вращался
    R_RangeA    = 20,   // ВОЛЬТ/ДЕЛ кан. 1
    R_RShiftA   = 21,   
    R_RangeB    = 22,   // ВОЛЬТ/ДЕЛ кан. 2
    R_RShiftB   = 23,
    R_TBase     = 24,   // ВРЕМЯ/ДЕЛ
    R_TShift    = 25,
    R_TrigLev   = 26,   // УРОВЕНЬ
    R_Set       = 27    // УСТАНОВКА
};

// Направления вращения регуляторов.
enum RegulatorDirection
{
    Direction_Left,     // вращение регулятора влево
    Direction_Right,    // вращение регулятора вправо
    Direction_None      // вращение отсутствует
};


#define NUM_REGULATORS (R_Set - R_RangeA + 1)


pchar NameButton(Key::E);
