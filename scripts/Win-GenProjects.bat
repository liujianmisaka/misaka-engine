@echo off
pushd ..\
call vendor\bin\premake\premake5.exe --file=premake5.lua vs2022
popd
PAUSE