mkdir c:\software\include\verve
mkdir c:\software\lib
copy src\*.h c:\software\include\verve
copy build\debug\*.lib c:\software\lib
copy build\release\*.lib c:\software\lib
copy build\debug\*.dll apps\bin
copy build\release\*.dll apps\bin
pause
