@echo off

if "%1" EQU "" goto HINT
if %1==debug   ( call :CLEAN_DEBUG & goto EXIT )
if %1==release ( call :CLEAN_RELEASE & goto EXIT )
if %1==all     ( call clean.bat debug & call clean.bat release & goto EXIT )

goto HINT

:CLEAN_DEBUG
    echo Clean Debug Keil
    %COMPILER% -c..\..\Device\Device.uvprojx -j0 -t"Debug"
    %COMPILER% -c..\..\DLoader\DLoader.uvprojx -j0 -t"Debug"
    %COMPILER% -c..\..\Panel\Panel.uvprojx -j0 -t"Debug"

    exit /b

:CLEAN_RELEASE
    echo Clean Release Keil
    %COMPILER% -c..\..\Device\Device.uvprojx -j0 -t"Release"
    %COMPILER% -c..\..\DLoader\DLoader.uvprojx -j0 -t"Release"
    %COMPILER% -c..\..\Panel\Panel.uvprojx -j0 -t"Release"

    exit /b

:HINT

    echo.
    echo Usage:
    echo       clean [debug^|release^|all]
    echo.
    goto EXIT

:EXIT
