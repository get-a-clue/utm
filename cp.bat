erase /Q /S include
erase /Q /S lib

copy src\_include\*.h include
copy src\_lib\*.lib lib

copy src\utm\*.h include
copy src\utm\*.hpp include
copy src\dualserver\*.h include
copy src\proxyserver\*.h include
copy src\proxyserver\*.hpp include
copy src\corefilters\*.h include
copy src\monitor\*.h include
copy src\monitor\*.hpp include

copy src\debug\*.lib lib
copy src\release\*.lib lib
copy src\debugU\*.lib lib
copy src\releaseU\*.lib lib

