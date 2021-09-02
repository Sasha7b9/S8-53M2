@echo off

if "%1" EQU "" goto HINT
if %1==device  ( call :LOAD Device  & goto EXIT )
if %1==dloader ( call :LOAD DLoader & goto EXIT )
if %1==panel   ( call :LOAD Panel   & goto EXIT )
if %1==all     ( call :LOAD Panel   & call :LOAD Device  & call :LOAD DLoader & goto EXIT )

goto HINT

:LOAD
    echo Load firmware to %1
    %COMPILER% -f ..\..\%1\%1.uvprojx -t"Release" -j0 -o ..\scripts\Keil\out.txt
    exit /b

:HINT

    echo.
    echo Usage:
    echo       load [device^|dloader^|panel^|all]
    echo.
    goto EXIT

:EXIT

