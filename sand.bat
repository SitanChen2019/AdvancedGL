@echo off

if  %1==cmake (
cmake %~dp0 -B %~dp0cmake-build -A Visual Studio 15 2017 [Win64]
goto :EOF
)


if %1==ide (
start %~dp0cmake-build\AdvancedGL.sln
goto :EOF
)


echo unknown parater: %1
:EOF