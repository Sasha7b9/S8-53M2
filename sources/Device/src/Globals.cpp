// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Globals.h"
#include "Display/DisplayTypes.h"
#include "Menu/MenuItems.h"


void *extraMEM = 0;


BitField gBF =
{
    0,  // ethTimeLastEthifInput
    0,  // cableEthIsConnected
    0,  // ethIsConnected

    0,  // showLevelRShift0
    0,  // showLevelRShift1
    0,  // showLevelTrigLev
    0,  // trigEnable
    0,  // drawRShiftMarkers
    1,  // needFinishDraw
    0,  // framesElapsed
    0,  // numDrawingSignals

    0,  // soundIsBeep
    0,  // soundWarnIsBeep
    0,  // buttonIsPressed
    
    1,  // panelIsRunning
    
    // Governor
    0,
    0,
    0,
    0,

    0,  // cableVCPisConnected
    0,  // connectToHost

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
    0,  // после установки имени и сохранения на флешку возвращаемся в основное рабочее окно
    0   // needForSaveToFlashDrive
};


StateFPGA gStateFPGA = 
{
    false,
    StateWorkFPGA::Stop,
    StateCalibration_None
};

pchar gStringForHint = 0;


void SetItemForHint(void *item)
{
    gStringForHint = 0;
    Hint::item = item;
}

int transmitBytes = 0;
