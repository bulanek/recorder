cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/Toolchain-arm-eabi-gcc.cmake -DCMAKE_INSTALL_PREFIX=../install ..
make install
cd ..
