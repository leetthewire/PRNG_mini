set buildpath=%~dp0
set sdkfolder=%buildpath%..\
echo %sdkfolder%

mkdir "%sdkfolder%build"
cd "%sdkfolder%build"

cmake -DSHARED_LIBRARY:STRING=False
cmake --build . --config Release

goto EOF 
:ERROR
echo Failed
cmd /k
exit /b 1

:EOF