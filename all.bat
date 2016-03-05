set BOOST_INCLUDE=c:\install\boost
set BOOST_LIB=c:\install\boost\stage\lib
set SolutionDir=s:\home\utm\src\

Setlocal EnableDelayedExpansion

mkdir c:\tmp
mkdir include
mkdir lib

rem erase /Q /S include
rem erase /Q /S lib

start.pl

cd src
rem svn up
update_currenttag.pl
rem svn commit utm\utm.h -m "Updated CurrentTag"
cd ..

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

FOR %%A IN (Debug DebugU DebugTest DebugUTest Release ReleaseU) DO (
  msbuild src\utm\utm.vcxproj /p:Configuration=%%A
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%%A\*.lib lib
  msbuild src\proxyserver\proxyserver.vcxproj /p:Configuration=%%A
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%%A\*.lib lib
  msbuild src\corefilters\corefilters.vcxproj /p:Configuration=%%A
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%%A\*.lib lib
  msbuild src\monitor\monitor.vcxproj /p:Configuration=%%A
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%%A\*.lib lib
  msbuild src\dualserver\dualserver.vcxproj /p:Configuration=%%A
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%%A\*.lib lib
)

FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO "src\DebugTest\%%A"
FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO "src\DebugUTest\%%A"

stop.pl

ziputm.pl

exit /b 0

:fail
echo "Building failure"
pause
exit /b 1
