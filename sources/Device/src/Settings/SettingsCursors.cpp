// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Settings.h"
#include "Display/Grid.h"
#include "Menu/Menu.h"
#include "SettingsCursors.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/TypesFPGA_old.h"
#include <math.h>



float sCursors_GetCursPosU(Chan::E ch, int numCur)
{
    return CURS_POS_U(ch, numCur) / (Grid::ChannelBottom() == Grid::FullBottom() ? 1.0f : 2.0f);
}


bool sCursors_NecessaryDrawCursors()
{
    return ((!CURS_CNTRL_U_IS_DISABLE(CURS_SOURCE)) || (!CURS_CNTRL_T_IS_DISABLE(CURS_SOURCE))) && 
        (CURS_SHOW || Menu::GetNameOpenedPage() == Page_SB_Curs);
}


const char* sCursors_GetCursVoltage(Chan::E source, int numCur, char buffer[20])
{
    float voltage = Math_VoltageCursor(sCursors_GetCursPosU(source, numCur), SET_RANGE(source), SET_RSHIFT(source));
    return Voltage2String(voltage, true, buffer);
}


const char* sCursors_GetCursorTime(Chan::E source, int numCur, char buffer[20])
{
    float time = Math_TimeCursor(CURS_POS_T(source, numCur), SET_TBASE);
        
    return Time2String(time, true, buffer);
}


const char* sCursors_GetCursorPercentsU(Chan::E source, char buffer[20])
{
    buffer[0] = 0;
    float dPerc = DELTA_U100(source);
    float dValue = fabs(sCursors_GetCursPosU(source, 0) - sCursors_GetCursPosU(source, 1));
    char bufferOut[20];
    char* percents = Float2String(dValue / dPerc * 100.0f, false, 5, bufferOut);
    strcat(buffer, percents);
    strcat(buffer, "%");
    return buffer;
}


const char* sCursors_GetCursorPercentsT(Chan::E source, char buffer[20])
{
    buffer[0] = 0;
    float dPerc = DELTA_T100(source);
    float dValue = fabs(CURS_POS_T0(source) - CURS_POS_T1(source));
    char bufferOut[20];
    char* percents = Float2String(dValue / dPerc * 100.0f, false, 6, bufferOut);
    strcat(buffer, percents);
    strcat(buffer, "%");
    return buffer;
}
