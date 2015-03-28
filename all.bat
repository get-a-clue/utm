mkdir c:\tmp
mkdir include
mkdir lib

rem erase /Q /S include
rem erase /Q /S lib

start.pl

rem cd src
rem svn up
rem update_currenttag.pl
rem svn commit utm\utm.h -m "Updated CurrentTag"
rem cd ..

call cph.bat

msbuild src/utm.sln /t:clean /p:Configuration=Debug
msbuild src/utm.sln /t:clean /p:Configuration=Release
msbuild src/utm.sln /t:clean /p:Configuration=DebugU
msbuild src/utm.sln /t:clean /p:Configuration=ReleaseU
msbuild src/utm.sln /t:clean /p:Configuration="DebugTest"
msbuild src/utm.sln /t:clean /p:Configuration="DebugUTest"

erase /Q /S "src\DebugTest"
erase /Q /S "src\DebugUTest"

call cp.bat

msbuild src/utm.sln /p:Configuration=Debug
if not %ERRORLEVEL%==0 goto fail

msbuild src/utm.sln /p:Configuration=Release
if not %ERRORLEVEL%==0 goto fail

msbuild src/utm.sln /p:Configuration=DebugU
if not %ERRORLEVEL%==0 goto fail

msbuild src/utm.sln /p:Configuration=ReleaseU
if not %ERRORLEVEL%==0 goto fail

msbuild src/utm.sln /p:Configuration="DebugTest"
if not %ERRORLEVEL%==0 goto fail

msbuild src/utm.sln /p:Configuration="DebugUTest"
if not %ERRORLEVEL%==0 goto fail

copy src\debug\*.lib lib
copy src\release\*.lib lib
copy src\debugU\*.lib lib
copy src\releaseU\*.lib lib

FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO "src\DebugTest\%%A"
FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO "src\DebugUTest\%%A"

stop.pl

ziputm.pl

exit /b 0

:fail
pause
exit /b 1
