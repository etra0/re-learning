@echo off
g++ -municode -I.\lib .\lib\process.cpp %1\main.cpp -o %1.exe