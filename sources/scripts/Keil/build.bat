@echo off

if "%1" EQU "" goto HINT
if "%2" EQU "" goto HINT
if %1==load    goto BUILD_LOAD
if %1==device  ( call :BUILD Device %2 & goto EXIT )
if %1==dloader ( call :BUILD DLoader %2 & goto EXIT )
if %1==panel   ( call :BUILD Panel %2 & goto EXIT )
if %1==all     ( call :BUILD Device %2 & call :BUILD DLoader %2 & call :BUILD Panel %2 & goto EXIT )

goto HINT

:BUILD_LOAD
    call build.bat %2 %3
    if %3==debug   goto LOAD_NOW
    if %3==release goto LOAD_NOW
    goto HINT
:LOAD_NOW
    call load.bat %2
    goto EXIT

:BUILD
    if %2==debug   ( call :BUILD_DEBUG %1 & exit /b )
    if %2==release ( call :BUILD_RELEASE %1 & exit /b )
    if %2==full    ( call :BUILD_DEBUG %1 & call :BUILD_RELEASE %1 & exit /b )
    goto HINT

:BUILD_DEBUG
    call :BUILD_COMMON %1 Debug
    exit /b

:BUILD_RELEASE
    call :BUILD_COMMON %1 Release
    if %1==Device ( echo copy S8-53M.bin h:\ & copy ..\..\Device\S8-53M.bin h:\ )
    exit /b

:BUILD_COMMON
    echo Keil : Build %1 %2
    if %2==Debug   ( %COMPILER% -b ..\..\%1\%1.uvprojx -t"Debug" -j0 -z -o %1.out)
    if %2==Release ( %COMPILER% -b ..\..\%1\%1.uvprojx -t"Release" -j0 -z -o %1.out)
    set BUILD_STATUS=%ERRORLEVEL%
    if %BUILD_STATUS%==0 goto BUILD_SUCCESS
    echo ERROR!!! %1 failed !!!
    type ..\..\%1\%1.out
    exit /b  

:BUILD_SUCCESS
    if "%3" EQU "" exit /b
    echo copy %3 h:\
    copy %3 h:\
    exit /b

:HINT
    echo.
    echo Usage:
    echo       build.bat [device^|dloader^|panel^|all] [debug^|release^|full]      - compile
    echo       build.bat load [device^|dloader^|panel^|all] [debug^|release^|full] - compile and load
    goto EXIT

:EXIT
