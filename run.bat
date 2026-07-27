@echo off
g++ src/*.cpp src/sqlite3.o -I sqlite/ -I include/ -o build/app
build\app.exe
