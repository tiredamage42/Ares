

pushd ..\

del *.sln
del /S *.vcxproj*

rmdir /Q /S .vs
rmdir /Q /S bin
rmdir /Q /S bin-int

call Premake\premake5.exe vs2019
popd


PAUSE
