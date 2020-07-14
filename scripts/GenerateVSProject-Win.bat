

call CleanVSProject.bat

pushd ..\
call vendor\bin\premake\premake5.exe vs2019
popd

IF %ERRORLEVEL% NEQ 0 (
    PAUSE
)