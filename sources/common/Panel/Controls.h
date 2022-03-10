// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct TypeLED
{
    enum E
    {
        // Вкл/выкл определяется так: если выкл, то посылается код лампочки, если включить, то устанавл. старший бит
        _None,
        Trig,   // 1 / 128 + 1
        RegSet, // 2 / 128 + 2
        ChanA,  // 3 / 128 + 3
        ChanB,  // 4 / 128 + 4
        Power   // 5
    };
};


struct Key { enum E
{                                   // Кнопки
        None,       // 
        F1,         // 1
        F2,         // 2
        F3,         // 3
        F4,         // 4
        F5,         // 5
        ChannelA,   // КАНАЛ 1
        ChannelB,   // КАНАЛ 2
        Time,       // РАЗВ
        Trig,       // СИНХР
        Cursors,    // КУРСОРЫ
        Measures,   // ИЗМЕР
        Display,    // ДИСПЛЕЙ
        Help,       // ПОМОЩЬ
        Start,      // ПУСК/СТОП
        Memory,     // ПАМЯТЬ
        Service,    // СЕРВИС
        Menu,       // МЕНЮ
        Power,      // ВЫКЛ
                                    // Ручки
        RangeA,     // ВОЛЬТ/ДЕЛ 1
        RangeB,     // ВОЛЬТ/ДЕЛ 2
        RShiftA,    // 
        RShiftB,    // 
        TBase,      // ВРЕМЯ/ДЕЛ
        TShift,     // 
        TrigLev,    // УРОВЕНЬ
        Setting,    // УСТАНОВКА
        Count
    };

    Key(E v = None) : value(v) { }

    E value;

    Key::E FromCode(uint16 code);
    uint16 ToCode(Key::E key);
    bool IsGovernor(Key::E key);
    bool Is(Key::E key);
    bool IsFunctional(Key::E key);
    pstring Name() const;
};

Key::E& operator++(Key::E &right);


struct Action { enum E {
        Down,         // Нажатие кнопки
        Up,           // Отпускание кнопки
        Long,         // "Длинное" нажатие
        RotateLeft,   // Поворот ручки влево
        RotateRight,  // Поворот ручки вправо
        Count
    } value;

    Action(E v = Count) : value(v) {}
    Action(uint8 v) : value((E)v) {}
    static Action::E FromCode(uint16 code);
    static uint16 ToCode(Action::E action);
    bool IsDown() const { return value == Down; }
    bool IsUp() const { return value == Up; }
    bool IsLong() const { return value == Long; }
    bool IsRotateLeft() const { return value == RotateLeft; }
    bool IsRotateRight() const { return value == RotateRight; }
    pchar Name() const;
};


struct KeyboardEvent
{
    KeyboardEvent(Key::E k = Key::Count, Action::E a = Action::Count) :key(k), action(a) {}
    KeyboardEvent(uint8 *buffer);

    Key key;
    Action action;

    bool Is(Key::E c) const           { return (c == key.value); };
    bool Is(Key::E c, Action::E a) const { return (key.value == c) && (action.value == a); };
       
    bool IsUp() const   { return (action.value == Action::Up); }
    bool IsDown() const { return (action.value == Action::Down); }
    bool IsLong() const { return (action.value == Action::Long); }
    // Возвращает true в случае отпускания кнопки или "длинного" нажатия
    bool IsRelease() const;

    // true, если функциональная клавиша
    bool IsFunctional() const;

    bool IsRotate() const { return (action.value == Action::RotateRight) || (action.value == Action::RotateLeft); }

    void Log() const;
    
    bool operator==(const KeyboardEvent &rhl) const
    {
        return (rhl.key.value == key.value) && (rhl.action.value == action.value);
    }
};
