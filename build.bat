@echo off
if exist edit.exe del edit.exe
tcc edit.c win32\cui.c
if exist edit.exe if exist tools\verpatch\verpatch.exe tools\verpatch\verpatch edit.exe /va 0.1.0.0 /pv 0.1.0.0 /s desc "Console mode text editor" /s copyright "(C) 2022 Orlando Llanes"
