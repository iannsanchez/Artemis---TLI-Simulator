@echo off
echo Compilando ARTEMIS - TLI Simulator...
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 main.c artemis.c fisica.c validaciones.c -o artemis.exe -lm
if errorlevel 1 (
    echo La compilacion fallo. Revise los mensajes anteriores.
    exit /b 1
)
echo Compilacion correcta. Ejecute artemis.exe