set BOOST_INCLUDE=c:\install\boost
set BOOST_LIB=c:\install\boost\stage\lib
set SolutionDir=s:\home\utmwork2\src\
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
   del done.%%A
   del fail.%%A
   del build.%%A
)

erase /Q /S "include\"
copy src\_include\*.* include\
FOR %%A IN (utm monitor corefilters proxyserver dualserver) DO (
  copy src\%%A\*.h include\
  copy src\%%A\*.hpp include\
)

FOR %%A IN (Debug DebugU Release ReleaseU) DO (
  start cmd /I /k all_config %%A
)

:wait
sleep 5
FOR %%A IN (Debug DebugU Release ReleaseU) DO (
   IF NOT EXIST done.%%A GOTO wait
)

FOR %%A IN (Debug DebugU Release ReleaseU) DO (
   IF EXIST fail.%%A (
      echo "Building failure for %%A"
      exit /b 1
   )
)

FOR %%A IN (DebugTest DebugUTest) DO (
  start cmd /I /k all_config %%A
)

:wait2
sleep 5
FOR %%A IN (DebugTest DebugUTest) DO (
   IF NOT EXIST done.%%A GOTO wait2
)

FOR %%A IN (DebugTest DebugUTest) DO (
   IF EXIST fail.%%A (
      echo "Building failure for %%A"
      exit /b 1
   )
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
