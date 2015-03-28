mkdir exe
mkdir include
mkdir lib

rem erase /Q /S include
rem erase /Q /S lib

start.pl

cd trunk\utm
svn up
update_currenttag.pl
svn commit utm\utm.h -m "Updated CurrentTag"
cd ..\..

call cph.bat

msbuild trunk/utm/utm.sln /t:clean /p:Configuration=Debug
msbuild trunk/utm/utm.sln /t:clean /p:Configuration=Release
msbuild trunk/utm/utm.sln /t:clean /p:Configuration=DebugU
msbuild trunk/utm/utm.sln /t:clean /p:Configuration=ReleaseU
msbuild trunk/utm/utm.sln /t:clean /p:Configuration="Debug Test"
msbuild trunk/utm/utm.sln /t:clean /p:Configuration="DebugU Test"

erase /Q /S "trunk\utm\Debug Test"
erase /Q /S "trunk\utm\DebugU Test"

call cp.bat

msbuild trunk/utm/utm.sln /p:Configuration=Debug
if not %ERRORLEVEL%==0 goto fail

msbuild trunk/utm/utm.sln /p:Configuration=Release
if not %ERRORLEVEL%==0 goto fail

msbuild trunk/utm/utm.sln /p:Configuration=DebugU
if not %ERRORLEVEL%==0 goto fail

msbuild trunk/utm/utm.sln /p:Configuration=ReleaseU
if not %ERRORLEVEL%==0 goto fail

msbuild trunk/utm/utm.sln /p:Configuration="Debug Test"
if not %ERRORLEVEL%==0 goto fail

msbuild trunk/utm/utm.sln /p:Configuration="DebugU Test"
if not %ERRORLEVEL%==0 goto fail

copy trunk\utm\debug\*.lib lib
copy trunk\utm\release\*.lib lib
copy trunk\utm\debugU\*.lib lib
copy trunk\utm\releaseU\*.lib lib

FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO "trunk\utm\Debug Test\%%A"
FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO "trunk\utm\DebugU Test\%%A"

stop.pl

ziputm.pl

exit /b 0

:fail
pause
exit /b 1
