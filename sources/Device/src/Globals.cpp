// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Globals.h"
#include "Display/DisplayTypes.h"
#include "Menu/MenuItems.h"


void *extraMEM = 0;


BitField gBF =
{
    // Governor
    0,
    0,

    // Flash drive
    0,  // flashDriveIsConnected
    1,  // cursorInDirs
    1,  // needRedrawFileManager

    0,  // settingsLoaded
    0,  // showHelpHints

    0,  // showDebugMenu

    GRID_BOTTOM, // topMeasures
 
    0,  // needOpenFileManager
    1,  // needClosePageSB
    0   // needResetSettings
};


GMemory gMemory =
{
    0,  // currentNumLatestSignal
    0,  // currentNumIntSignal
    0,  // showAlways
    0,  // runningFPGAbeforeSmallButtons
    0,  // exitFromIntToLast
    0,  // ����� ��������� ����� � ���������� �� ������ ������������ � �������� ������� ����
    0   // needForSaveToFlashDrive
};


StateFPGA gStateFPGA = 
{
    false,
    StateWorkFPGA::Stop,
    StateCalibration_None
};


int transmitBytes = 0;
