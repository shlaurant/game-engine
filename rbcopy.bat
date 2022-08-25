@echo off

if not exist build mkdir build

rem copy assets and .dll files to output mkdir
robocopy "./resource" "./build/bin/assets" /E
robocopy "./windows/bin/" "./build/bin" /E