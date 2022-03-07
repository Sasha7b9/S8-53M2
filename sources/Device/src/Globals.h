// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Panel/Controls.h"
#include "Settings/SettingsTypes.h"
#include "Settings/SettingsChannel.h"
#include "Settings/SettingsTime.h"

struct DataSettings;


#define EXIT_FROM_INT_TO_LAST           (gMemory.exitFromIntToLast)
#define NEED_SAVE_TO_DRIVE              (gMemory.needForSaveToFlashDrive)

#define EXIT_FROM_SET_NAME_TO           (gMemory.exitFromModeSetNameTo)
#define RETURN_TO_MAIN_MENU             0U
#define RETURN_TO_LAST_MEM              1U
#define RETURN_TO_INT_MEM               2U
#define RETURN_TO_DISABLE_MENU          3U
#define EXIT_FROM_SET_NAME_TO_MAIN_MENU (EXIT_FROM_SET_NAME_TO == RETURN_TO_MAIN_MENU)
#define EXIT_FROM_SET_NAME_TO_LAST      (EXIT_FROM_SET_NAME_TO == RETURN_TO_LAST_MEM)
#define EXIT_FROM_SET_NAME_TO_INT       (EXIT_FROM_SET_NAME_TO == RETURN_TO_INT_MEM)
#define EXIT_FROM_SET_NAME_TO_DIS_MENU  (EXIT_FROM_SET_NAME_TO == RETURN_TO_DISABLE_MENU)

struct GMemory
{
    uint    exitFromIntToLast               : 1;    // Если 1, то выходить из страницы внутренней памяти нужно не стандартно, а в меню последних
    uint    exitFromModeSetNameTo           : 2;    // Куда возвращаться из окна установки имени при сохранении : 0 - в основное меню, 1 - в окно 
                                                    // последних, 2 - в окно Внутр ЗУ, 3 - в основно окно в выключенным меню
    uint    needForSaveToFlashDrive         : 1;    // Если 1, то нужно сохранить после отрисовки на флешку.
};


extern GMemory gMemory;


enum StateCalibration
{
    StateCalibration_None,
    StateCalibration_ADCinProgress,
    StateCalibration_RShift0start,
    StateCalibration_RShift0inProgress,
    StateCalibration_RShift1start,
    StateCalibration_RShift1inProgress,
    StateCalibration_ErrorCalibration0,
    StateCalibration_ErrorCalibration1
};

struct StateWorkFPGA { enum E {
        Stop,    // СТОП - не занимается считыванием информации.
        Wait,    // Ждёт поступления синхроимпульса.
        Work,    // Идёт работа.
        Pause    // Это состояние, когда временно приостановлен прибор, например, для чтения данных или для 
                 // записи значений регистров.
    };

    E value;

    StateWorkFPGA(E v = Stop) : value(v) {}

    static StateWorkFPGA::E GetCurrent() { return current; }
    static void SetCurrent(StateWorkFPGA::E v) { current = v; }

private:

    static E current;
};


struct StateFPGA
{
    bool             needCalibration;               // Установленное в true значение означает, что необходимо произвести калибровку.
    StateWorkFPGA    stateWorkBeforeCalibration;
    StateCalibration stateCalibration;              // Текущее состояние калибровки. Используется в процессе калибровки.
};


extern StateFPGA gStateFPGA;

extern void *extraMEM;      // Это специальный указатель. Используется для выделения памяти переменным, которые не нужны всё время выполения программы,
                            // но нужны болеее чем в одной функции. Перед использованием с помощью вызова malloc() выделяется необходимое количество
                            // памяти, которая затем освобождается вызвом free()

#define MALLOC_EXTRAMEM(NameStruct, name)   extraMEM = malloc(sizeof(NameStruct));    \
                                            NameStruct *name = (NameStruct*)extraMEM
#define ACCESS_EXTRAMEM(NameStruct, name)   NameStruct *(name) = (NameStruct*)extraMEM //-V1003
#define FREE_EXTRAMEM()                     free(extraMEM)

extern int transmitBytes;
