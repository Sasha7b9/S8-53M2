@echo off

set _COMPILER_=c:\Keil_v5\UV4\UV4
set _PROJECT_DEVICE_=..\..\Device\Device.uvprojx
set _PROJECT_PANEL_=..\..\Panel\Panel.uvprojx
set _PROJECT_LOADER_=..\..\Loader\Loader.uvprojx

rem 1, если требуется очистка
set isClean=0

rem 1, если требуется сборка
set isBuild=0

rem 1, если требуется загрузка
set isLoad=0

rem 1, если нужно обрабатывать загрузчик
set need_loader=0

rem 1, если нужно обрабатывать панель
set need_panel=0

rem 1, если нужно обрабатывать устройство
set need_device=0

if "%1" equ "clear"   set isClean=1 &                  goto CHECK_ON_LOAD
if "%1" equ "build"   set isBuild=1 &                  goto CHECK_ON_LOAD
if "%1" equ "rebuild" set isClean=1 & set isBuild=1 &  goto CHECK_ON_LOAD
if "%1" equ "load"    set isLoad=1 &                   goto CHECK_ON_LOAD
goto HINT

rem Проверка на то, нужно ли загружать (второй параметр - "load")
:CHECK_ON_LOAD
if "%2" equ "load"   set isLoad=1 &                                             goto CHECK_TARGET
if "%2" equ "panel"  set need_panel=1 &                                         goto CLEANING
if "%2" equ "loader" set need_loader=1 &                                        goto CLEANING
if "%2" equ "device" set need_device=1 &                                        goto CLEANING
if "%2" equ "all"    set need_panel=1 & set need_loader=1 & set need_device=1 & goto CLEANING
goto HINT

:CHECK_TARGET
if "%3" equ "panel"  set need_panel=1 &                                         goto CLEANING
if "%3" equ "loader" set need_loader=1 &                                        goto CLEANING
if "%3" equ "device" set need_device=1 &                                        goto CLEANING
if "%3" equ "all"    set need_panel=1 & set need_loader=1 & set need_device=1 & goto CLEANING
goto HINT

:CLEANING
if %isClean%==0 goto BUILDING
if %need_loader%==0 goto CLEANING_PANEL
echo Cleaning Loader ...
%_COMPILER_% -c %_PROJECT_LOADER_% -j0
:CLEANING_PANEL
if %need_panel%==0 goto CLEANING_DEVICE
echo Cleaning Panel ...
%_COMPILER_% -c %_PROJECT_PANEL_% -j0
:CLEANING_DEVICE
if %need_device%==0 goto BUILDING
echo Cleaning Device ...
%_COMPILER_% -c %_PROJECT_DEVICE_% -j0
:BUILDING
if %isBuild%==0 goto LOADING
if %need_loader%==0 goto BUILDING_PANEL
echo Building Loader ...
%_COMPILER_% -b %_PROJECT_LOADER_% -j0 -o Loader.out
if %ERRORLEVEL%==0 goto BUILDING_PANEL
echo ERROR!!! Build failed !!!
type ..\..\Loader\Loader.out
:BUILDING_PANEL
if %need_panel%==0 goto BUILDING_DEVICE
echo Building Panel ...
%_COMPILER_% -b %_PROJECT_PANEL_% -j0 -o Panel.out
if %ERRORLEVEL%==0 goto BUILDING_DEVICE
echo ERROR!!! Build failed !!!
type ..\..\Panel\Panel.out
:BUILDING_DEVICE
if %need_device%==0 goto LOADING
echo on
del /Q ..\..\Device\*.bin
echo off
:BUILD_DEVICE
echo Building Device ...
%_COMPILER_% -b %_PROJECT_DEVICE_% -j0 -o Device.out
if %ERRORLEVEL%==0 goto COPY_FILE
echo ERROR!!! Build failed !!!
type ..\..\Device\Device.out
:COPY_FILE
call :CALL_COPY h:\
call :CALL_COPY g:\
call :CALL_COPY z:\
:LOADING
if %isLoad%==0 goto EXIT
if %need_loader%==0 goto LOADING_PANEL
echo Loading Loader ...
%_COMPILER_% -f %_PROJECT_LOADER_% -j0 -o Loader.out
if %ERRORLEVEL%==0 goto LOADING_PANEL
echo Error loading firmware to Loader
type ..\..\Loader\Loader.out
:LOADING_PANEL
if %need_panel%==0 goto LOADING_DEVICE
echo Loading Panel ...
%_COMPILER_% -f %_PROJECT_PANEL_% -j0 -o Panel.out
if %ERRORLEVEL%==0 goto LOADING_DEVICE
echo Error loading firmware to Panel
type ..\..\Panel\Panel.out
:LOADING_DEVICE
if %need_device%==0 goto EXIT
echo Loading Device ...
%_COMPILER_% -f %_PROJECT_DEVICE_% -j0 -o Device.out
if %ERRORLEVEL%==0 goto EXIT
echo Error loading firmware to Device
type ..\..\Device\Device.out
goto EXIT
:HINT
echo.
echo Using assembly.bat:
echo                    assembly.bat [clear^|build^|rebuild^|load] [load] [device^|loader^|panel^|all]
goto EXIT
:CALL_COPY
    if not exist %1 exit /b
    echo on
    copy /Y ..\..\Device\S8-53.bin %1
    echo off
    exit /b
:EXIT