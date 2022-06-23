@echo off
if exist edit.exe del edit.exe
if exist e.exe del e.exe
if exist r.exe del r.exe

tcc edit.c win32\cui.c
tcc e.c
tcc r.c

if exist edit.exe if exist tools\verpatch\verpatch.exe tools\verpatch\verpatch edit.exe /va 0.1.0.0 /pv 0.1.0.0 /s desc "Console mode text editor" /s copyright "(C) 2022 Orlando Llanes"

if exist e.exe if exist tools\verpatch\verpatch.exe tools\verpatch\verpatch e.exe /va 0.1.0.0 /pv 0.1.0.0 /s desc "Console mode text editor" /s copyright "(C) 2022 Orlando Llanes"
