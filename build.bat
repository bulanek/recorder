set MAKE=C:\git\mf_devkit\Tools\MinGW\bin\mingw32-make.exe

set SOURCE_DIR=%~dp0
mkdir %TEMP%\build
cd /d %TEMP%\build
cmake  -B%TEMP%\build -DCMAKE_TOOLCHAIN_FILE=%SOURCE_DIR%/cmake/Toolchain-arm-eabi-gcc.cmake -DCMAKE_INSTALL_PREFIX=%TEMP%\install  -DCMAKE_MAKE_PROGRAM=%MAKE%  -DFF13_DMA=ON -G "Unix Makefiles" %SOURCE_DIR%
%MAKE% VERBOSE=1 -j 8 

%MAKE% install VERBOSE=1 -j 8

cd /d %~dp0
