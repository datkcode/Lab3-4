@echo off
setlocal enabledelayedexpansion

title STM32 Proteus Auto-Loader - Lab 3

echo ===================================================
echo   STM32 Proteus Auto-Build and Simulation [Lab 3]
echo ===================================================
echo.

rem -----------------------------------------------------
rem 1. DYNAMIC CONFIGURATION AND TARGET RESOLUTION
rem -----------------------------------------------------
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build"

rem Priority 1: Target name from argument (e.g. run.bat Lab3)
set "TARGET_NAME=%~1"

rem Priority 2: Auto-detect Proteus design file (*.pdsprj)
if "%TARGET_NAME%"=="" (
    for %%F in ("%PROJECT_DIR%*.pdsprj") do (
        if exist "%%F" (
            set "TARGET_NAME=%%~nF"
            set "PDS_PROJECT=%%F"
        )
    )
)

rem Priority 3: Auto-detect STM32CubeMX file (*.ioc)
if "%TARGET_NAME%"=="" (
    for %%F in ("%PROJECT_DIR%*.ioc") do (
        if exist "%%F" (
            set "TARGET_NAME=%%~nF"
        )
    )
)

rem Priority 4: Fallback to directory name
if "%TARGET_NAME%"=="" (
    for %%I in ("%PROJECT_DIR:~0,-1%") do set "TARGET_NAME=%%~nxI"
)

rem Fallback if still empty
if "%TARGET_NAME%"=="" set "TARGET_NAME=Lab3"

rem Set Proteus project path
if not defined PDS_PROJECT (
    if exist "%PROJECT_DIR%%TARGET_NAME%.pdsprj" (
        set "PDS_PROJECT=%PROJECT_DIR%%TARGET_NAME%.pdsprj"
    )
)

set "HEX_FILE=%BUILD_DIR%\%TARGET_NAME%.hex"
set "HEX_ROOT=%PROJECT_DIR%%TARGET_NAME%.hex"

rem Set toolchain argument
set "TOOLCHAIN_ARG="
if exist "%PROJECT_DIR%cmake\gcc-arm-none-eabi.cmake" (
    set "TOOLCHAIN_ARG=-DCMAKE_TOOLCHAIN_FILE="%PROJECT_DIR%cmake\gcc-arm-none-eabi.cmake""
)

echo [INFO] Project directory : %PROJECT_DIR%
echo [INFO] Detected target   : %TARGET_NAME%
if defined PDS_PROJECT (
    echo [INFO] Proteus project   : %PDS_PROJECT%
) else (
    echo [INFO] Proteus project   : (Not found, launching Proteus directly)
)
echo.

rem -----------------------------------------------------
rem 2. LOCATE PROTEUS 8 EXECUTABLE
rem -----------------------------------------------------
set "PROTEUS_EXE="

rem Check system registry
for /f "tokens=2* skip=2" %%a in ('reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\PDS.EXE" /ve 2^>nul') do (
    if exist "%%b" set "PROTEUS_EXE=%%b"
)
if not defined PROTEUS_EXE (
    for /f "tokens=2* skip=2" %%a in ('reg query "HKLM\SOFTWARE\Labcenter Electronics\Proteus 8 Professional" /v "InstallPath" 2^>nul') do (
        if exist "%%b\BIN\PDS.EXE" set "PROTEUS_EXE=%%b\BIN\PDS.EXE"
    )
)
if not defined PROTEUS_EXE (
    for /f "tokens=2* skip=2" %%a in ('reg query "HKLM\SOFTWARE\WOW6432Node\Labcenter Electronics\Proteus 8 Professional" /v "InstallPath" 2^>nul') do (
        if exist "%%b\BIN\PDS.EXE" set "PROTEUS_EXE=%%b\BIN\PDS.EXE"
    )
)

rem Check default installation paths
if not defined PROTEUS_EXE (
    if exist "C:\Program Files (x86)\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE" (
        set "PROTEUS_EXE=C:\Program Files (x86)\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE"
    ) else if exist "C:\Program Files\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE" (
        set "PROTEUS_EXE=C:\Program Files\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE"
    ) else if exist "D:\Program Files (x86)\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE" (
        set "PROTEUS_EXE=D:\Program Files (x86)\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE"
    ) else if exist "D:\Program Files\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE" (
        set "PROTEUS_EXE=D:\Program Files\Labcenter Electronics\Proteus 8 Professional\BIN\PDS.EXE"
    )
)

rem Check PATH
if not defined PROTEUS_EXE (
    for %%X in (PDS.EXE) do (
        if not "%%~$PATH:X"=="" set "PROTEUS_EXE=%%~$PATH:X"
    )
)

rem -----------------------------------------------------
rem 3. BUILD FIRMWARE
rem -----------------------------------------------------
echo [1/3] Compiling firmware [%TARGET_NAME%]...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

set "CMAKE_GEN=MinGW Makefiles"
where ninja >nul 2>nul
if %ERRORLEVEL%==0 set "CMAKE_GEN=Ninja"

if not exist "%BUILD_DIR%\Makefile" if not exist "%BUILD_DIR%\build.ninja" (
    echo [INFO] Configuring CMake with generator [%CMAKE_GEN%]...
    cmake -B "%BUILD_DIR%" -G "%CMAKE_GEN%" %TOOLCHAIN_ARG%
    if errorlevel 1 goto BUILD_ERROR
)

cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 goto BUILD_ERROR

set "ELF_FILE=%BUILD_DIR%\%TARGET_NAME%.elf"
if not exist "%ELF_FILE%" (
    for /r "%BUILD_DIR%" %%F in (*.elf) do (
        set "ELF_FILE=%%F"
    )
)

if exist "%ELF_FILE%" (
    echo [INFO] Generating/updating HEX file from ELF...
    arm-none-eabi-objcopy -O ihex "%ELF_FILE%" "%HEX_FILE%"
)

if not exist "%HEX_FILE%" (
    for /r "%BUILD_DIR%" %%F in (*.hex) do (
        set "HEX_FILE=%%F"
        set "HEX_ROOT=%PROJECT_DIR%%%~nxF"
    )
)

if not exist "%HEX_FILE%" (
    echo [ERROR] Cannot find or generate %HEX_FILE%
    goto BUILD_ERROR
)

copy /y "%HEX_FILE%" "%HEX_ROOT%" >nul
<nul set /p="%HEX_ROOT%" | clip

echo.
echo [2/3] SUCCESS: Firmware compiled successfully!
echo       Build HEX : "%HEX_FILE%"
echo       Root HEX  : "%HEX_ROOT%"
echo.

rem -----------------------------------------------------
rem 4. RELOAD OR LAUNCH PROTEUS
rem -----------------------------------------------------
echo [3/3] Checking Proteus 8 status...

tasklist /FI "IMAGENAME eq PDS.exe" 2>NUL | find /I /N "PDS.exe">NUL
if "%ERRORLEVEL%"=="0" goto PROTEUS_RUNNING

:PROTEUS_NOT_RUNNING
if "%PROTEUS_EXE%"=="" (
    echo [WARNING] Proteus 8 is not running and PDS.EXE path was not found.
    echo           Please open Proteus manually and load: "%HEX_ROOT%"
    goto SUCCESS_END
)
if exist "%PDS_PROJECT%" (
    echo [INFO] Proteus is not running. Launching design file: "%PDS_PROJECT%"
    start "" "%PROTEUS_EXE%" "%PDS_PROJECT%"
) else (
    echo [INFO] Proteus is not running. Launching Proteus 8...
    start "" "%PROTEUS_EXE%"
)
goto SUCCESS_END

:PROTEUS_RUNNING
echo [INFO] Proteus 8 is ALREADY RUNNING!
echo [INFO] Triggering simulation reload...

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$wshell = New-Object -ComObject WScript.Shell;" ^
    "$proc = Get-Process -Name PDS -ErrorAction SilentlyContinue | Select-Object -First 1;" ^
    "$ok = if ($proc) { $wshell.AppActivate($proc.Id) } else { $false };" ^
    "if (-not $ok) { $ok = $wshell.AppActivate('Proteus'); }" ^
    "if ($ok) {" ^
        "Start-Sleep -Milliseconds 250;" ^
        "$wshell.SendKeys('+ ');" ^
        "Start-Sleep -Milliseconds 350;" ^
        "$wshell.SendKeys(' ');" ^
    "}"

:SUCCESS_END
echo.
echo ===================================================
echo   CODE BUILD AND AUTO-RELOAD SUCCESSFUL!
echo ===================================================
echo   HEX Path copied to clipboard:
echo   %HEX_ROOT%
echo ===================================================
echo.
goto END

:BUILD_ERROR
echo.
echo ===================================================
echo   [BUILD ERROR] COMPILATION FAILED!
echo ===================================================
echo   Please check the error output above.
echo ===================================================
echo.

:END
pause
