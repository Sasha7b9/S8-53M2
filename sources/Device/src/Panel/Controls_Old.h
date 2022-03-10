// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Èäåíòèôèêàòîğû êíîïîê.
struct KeyOld
{
    enum E
    {
        Empty    = 0,    // êíîïêà íå íàæàòà
        ChannelA = 1,    // ÊÀÍÀË 1
        Service  = 2,    // ÑÅĞÂÈÑ
        ChannelB = 3,    // ÊÀÍÀË 2
        Display  = 4,    // ÄÈÑÏËÅÉ
        Time     = 5,    // ĞÀÇÂ
        Memory   = 6,    // ÏÀÌßÒÜ
        Trig     = 7,    // ÑÈÍÕĞ
        Start    = 8,    // ÏÓÑÊ/ÑÒÀĞÒ
        Cursors  = 9,    // ÊÓĞÑÎĞÛ
        Measures = 10,   // ÈÇÌÅĞ
        Power    = 11,   // ÏÈÒÀÍÈÅ
        Help     = 12,   // ÏÎÌÎÙÜ
        Menu     = 13,   // ÌÅÍŞ
        F1       = 14,   // 1
        F2       = 15,   // 2
        F3       = 16,   // 3
        F4       = 17,   // 4
        F5       = 18,   // 5
        Count            // îáùåå êîëè÷åñòâî êíîïîê
    };

    static pchar Name(KeyOld::E);

    // Âîçâğàùàåò true, åñëè button - ôóíêöèîíàëüíàÿ êëàâèøà [1..5].
    static bool IsFunctional(KeyOld::E);
};

KeyOld::E& operator++(KeyOld::E &);

// Èäåíòèôèêàòîğû ğåãóëÿòîğîâ.
struct RegulatorOld
{
    enum E
    {
        Empty   = 0,    // ğåãóëÿòîğ íå âğàùàëñÿ
        RangeA  = 20,   // ÂÎËÜÒ/ÄÅË êàí. 1
        RShiftA = 21,
        RangeB  = 22,   // ÂÎËÜÒ/ÄÅË êàí. 2
        RShiftB = 23,
        TBase   = 24,   // ÂĞÅÌß/ÄÅË
        TShift  = 25,
        TrigLev = 26,   // ÓĞÎÂÅÍÜ
        Set     = 27    // ÓÑÒÀÍÎÂÊÀ
    };

    // Âîçâğàùàåò true, åñëè ğåãóëÿòîğ ïîâîğà÷èâàåòñÿ âëåîâ
    static bool IsLeft(uint16 data) { return (data >= RangeA && data <= Set); }

    // Âîçâğàùàåò false, åñëè ğåãóëÿòîğ ïîâîğà÷èâàåòñÿ âïğàâî
    static bool IsRight(uint16 data) { return ((data & 0x7f) >= 20) && ((data & 0x7f) <= 27); }
};
