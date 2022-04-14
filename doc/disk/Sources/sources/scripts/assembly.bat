@echo off
if "%1" equ "build" goto BUILD
if "%1" equ "full" goto FULL
goto HINT
:BUILD
cd keil
call assembly build all
cd ..
cd vs_keil
call assembly build
cd ..
cd vs_gui
call assembly build
cd ..
goto EXIT
:FULL
cd keil
call assembly rebuild all
cd ..
cd vs_keil
call assembly full
cd ..
cd vs_gui
call assembly full
cd ..
goto EXIT
:HINT
echo.
echo Using assembly.bat:
echo                    assembly.bat [build^|full]
echo.
goto EXIT
:EXIT