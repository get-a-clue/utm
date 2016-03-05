Setlocal EnableDelayedExpansion

Echo "Building configuration %1"

""C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"" x86

  msbuild src\utm\utm.vcxproj /p:Configuration=%1 |tee -a build.%1
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%1\*.lib lib
  msbuild src\proxyserver\proxyserver.vcxproj /p:Configuration=%1 |tee -a build.%1
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%1\*.lib lib
  msbuild src\corefilters\corefilters.vcxproj /p:Configuration=%1 |tee -a build.%1
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%1\*.lib lib
  msbuild src\monitor\monitor.vcxproj /p:Configuration=%1 |tee -a build.%1
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%1\*.lib lib
  msbuild src\dualserver\dualserver.vcxproj /p:Configuration=%1 |tee -a build.%1
  if not !ERRORLEVEL!==0 goto fail
  copy /y src\%1\*.lib lib

echo 0 > done.%1
exit 0
exit /b 0

:fail
echo "Building failure configuration %1"  |tee -a build.%1
echo 1 > done.%1
echo 1 > fail.%1
exit 1
exit /b 1
