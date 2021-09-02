@echo off

if "%1" EQU "" goto HINT
set res=0
if %1==device  set res=1
if %1==dloader set res=1
if %1==panel   set res=1
if %res%==1    ( call clean.bat %1 & call build.bat %1 & goto EXIT )
if %1==all     ( call rebuild.bat device & call rebuild.bat dloader & goto EXIT )

goto HINT

:HINT
    echo.
    echo Usage:
    echo       rebuild.bat [device^|dloader^|panel^|all]
    goto EXIT

:EXIT
