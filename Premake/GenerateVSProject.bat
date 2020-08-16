

REM call CleanVSProject-Win.bat
pushd ..\
del /S *.vcxproj*
del *.sln
rmdir /Q /S .vs
rmdir /Q /S bin
rmdir /Q /S bin-int

REM call vendor\bin\premake\premake5.exe vs2019
call Premake\premake5.exe vs2019
popd


PAUSE
