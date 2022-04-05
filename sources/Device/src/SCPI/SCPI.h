// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/VCP/VCP.h"
#include "Hardware/LAN/LAN.h"
#include "Utils/Text/Text.h"


#define FIRST_SYMBOLS(word) (FirstSymbols(&buffer, word))

#define IF_REQUEST(sequence) if(FirstSymbols(&buffer, "?")) { sequence; return true; }

#define SCPI_CYCLE(func)                                    \
    const MapElement *it = map;                             \
    while (it->key)                                         \
    {                                                       \
        if FIRST_SYMBOLS(it->key) { func; return true; }    \
        it++;                                               \
    }


namespace SCPI
{
    struct MapElement
    {
        char *key;
        uint8 value;

        uint8 GetValue(Word *key) const;      // Если значение не найдено, возвращеется 255;
    };

    struct StructCommand
    {
        typedef bool(*pFuncBC)(pchar);

        char    *name;
        pFuncBC  func;
    };

    void AppendNewData(const uint8 *buffer, int length);
    void Update();

    void SendBuffer(const uint8 *buffer, int size);
    void SendFormat(pchar format, ...);

    bool ProcessingCommand(const StructCommand *, pchar);

    // Возвращает true, если первые символы в buffer повторяют word (без учёта завершающего нуля).
    // В этом случае записывает по *buffer адрес следующего за одинаоковыми символами
    bool FirstSymbols(pchar *buffer, pchar word);

    bool ProcessDISPLAY(pchar);
    bool ProcessCHANNEL(pchar);
    bool ProcessTRIG(pchar);
    bool ProcessTBASE(pchar);
    bool ProcessSpeedTest(pchar);

    namespace COMMON
    {
        bool IDN(pchar);
        bool RUN(pchar);
        bool STOP(pchar);
        bool RESET(pchar);
        bool AUTOSCALE(pchar);
        bool REQUEST(pchar);
    };

    namespace CHANNEL
    {
        bool INPUT(pchar);
        bool COUPLE(pchar);
        bool FILTR_(pchar);
        bool INVERSE(pchar);
        bool RANGE_(pchar);
        bool OFFSET(pchar);
        bool FACTOR(pchar);
    };

    namespace CONTROL
    {
        bool KEY(pchar);
        bool GOVERNOR(pchar);
    };

    namespace DISPLAY
    {
        bool AUTOSEND(pchar);
        bool MAPPING(pchar);
        bool ACCUM(pchar);
        bool ACCUM_NUMBER(pchar);
        bool ACCUM_MODE(pchar);
        bool ACCUM_CLEAR(pchar);
        bool AVERAGE(pchar);
        bool AVERAGE_NUMBER(pchar);
        bool AVERAGE_MODE(pchar);
        bool MINMAX(pchar);
        bool FILTR_(pchar);
        bool FPS(pchar);
        bool WINDOW(pchar);
        bool GRID(pchar);
        bool GRID_TYPE(pchar);
        bool GRID_BRIGHTNESS(pchar);
    };

    namespace TBASE_
    {
        bool RANGE_(pchar);
        bool OFFSET(pchar);
        bool SAMPLING(pchar);
        bool PEACKDET(pchar);
        bool TPOS_(pchar);
        bool SELFRECORDER(pchar);
        bool FUNCTIMEDIV(pchar);
    };

    namespace TRIGGER
    {
        bool MODE(pchar);
        bool SOURCE(pchar);
        bool POLARITY(pchar);
        bool INPUT(pchar);
        bool FIND(pchar);
        bool OFFSET(pchar);
    };
};
