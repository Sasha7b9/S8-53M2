// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "FPGA_Types.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;

    void Init();

    // Установить количество считываемых сигналов в секунду.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();

    // Запись в регистр ПЛИС нового значения. restart - true означает, что после записи нужно запусить режим измерений, если до этого прибор не находился в режиме паузы.
    void WriteToHardware(uint8 *address, uint8 value, bool restart);

    // Запускает цикл сбора информации.
    void OnPressStartStop();

    // Запуск процесса сбора информации.
    void Start();

    // Прерывает процесс сбора информации.
    void Stop(bool pause);

    // Возвращает true, если прибор находится не в процессе сбора информации.
    bool IsRunning();

    // Сохраняет текущие настройки. Потом их можно восстановить функцией FPGA_RestoreState().
    void SaveState();

    // Восстанавливает настройки, ранее сохранённые функцией FPGA_SaveState().
    void RestoreState();

    // Получить значение частоты.
    float GetFreq();

    // Удаляет данные. Нужно для режима рандомизаотра, где информация каждого цикла не является самостоятельной.
    void ClearData();

    // Установить дополнительное смещение. Нужно для правильной расстановки точек в режиме рандомизатора.
    void SetAdditionShift(int shift);

    // Возвращает true,если все точки получены в режиме рандомизатора.
    bool AllPointsRandomizer();

    // Установить количество измерений, по которым будут рассчитываться ворота в режиме рандомизатора.
    void SetNumberMeasuresForGates(int number);

    // Принудительно запустить синхронизацию.
    void SwitchingTrig();

    // Запустить процесс поиска сигнала.
    void StartAutoFind();

    // Установить временную паузу после изменения ручек - чтобы смещённый сигнал зафиксировать на некоторое время
    void TemporaryPause();

    // Найти и установить уровень синхронизации по последнему считанному сигналу
    void FindAndSetTrigLevel();

    // Установить режим канала по входу.
    void SetModeCouple(Chan::E, ModeCouple::E modeCoupe);

    // Включить/выключить фильтр на входе канала.
    void EnableChannelFiltr(Chan::E, bool enable);

    // Включить/выключить режим пикового детектора.
    void SetPeackDetMode(PeackDetMode peackDetMode);

    // Включить/выключить калибратор.
    void SetCalibratorMode(CalibratorMode calibratorMode);

    // Загрузить в аппарат коэффициенты калибровки каналов.
    void LoadKoeffCalibration(Chan::E);

    // Запуск функции калибровки.
    void ProcedureCalibration();
};
