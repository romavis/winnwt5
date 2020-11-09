@echo on
rem run in build directory
rem cd ..
set src_dir="..\"
set /p var= < %src_dir%build.txt 
set /a var=%var%+1 
echo %var% > %src_dir%build.txt
echo #define BUILDNO %var% > %src_dir%build.h
rem echo #define BUILDNO %var%
rem echo %var%

rem pause