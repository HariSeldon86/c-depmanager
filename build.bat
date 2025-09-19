@echo off
:: Enable ANSI escape sequences
reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f >nul 2>&1

:: Define color codes
set "GREEN=[32m"
set "YELLOW=[33m"
set "RED=[31m"
set "RESET=[0m"

SET SRC=src/*.c
SET OUT=build/c-depmanager.exe
SET CC=gcc
SET CFLAGS=-Wall -Wextra -O2
SET LDFLAGS=
SET INCLUDES=-Iinclude
SET LIBS=
@REM SET RM=del /Q
@REM SET RMDIR=rd /S /Q



IF NOT EXIST build mkdir build

call :color_echo "[INFO] Compiling %SRC% to %OUT%..." "Green"
%CC% %CFLAGS% %INCLUDES% -o %OUT% %SRC% %LDFLAGS% %LIBS%
IF ERRORLEVEL 1 (
    call :color_echo "[ERROR] Build failed!" "Red"
    EXIT /B 1
) ELSE (
    @REM pause
    call :color_echo "[INFO] Running %OUT%..." "Green"
    call "%OUT%"
    IF ERRORLEVEL 1 (
        call :color_echo "[ERROR] Execution failed!" "Red"
        EXIT /B 1
    )
)

goto :eof

:color_echo
powershell -Command "Write-Host '%~1' -ForegroundColor %~2"
goto :eof