# Targeting an embedded system, no OS.
set(CMAKE_SYSTEM_NAME Generic)

#Not used, only for possibility of further usage in projects
set(CMAKE_SYSTEM_PROCESSOR arm)

set(BIN_SUFFIX)
set (ARM_GCC_HOME_REL "/usr" )
if (WIN32)
	set (ARM_GCC_HOME_REL "C:/git/mf_devkit/Tools/gnu_tools_arm_embedded_8_2018_q4_major")
set (BIN_SUFFIX ".exe")
endif()

get_filename_component(ARM_GCC_HOME ${ARM_GCC_HOME_REL} ABSOLUTE)

set(ARM_GCC_BIN_DIR ${ARM_GCC_HOME}/bin )
set(ARM_GCC_INC_DIR ${ARM_GCC_HOME}/arm-none-eabi/include)
set(ARM_GCC_LIB_DIR ${ARM_GCC_HOME}/arm-none-eabi/lib)


SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

SET(CMAKE_C_COMPILER ${ARM_GCC_BIN_DIR}/arm-none-eabi-gcc${BIN_SUFFIX} )
SET(CMAKE_CXX_COMPILER ${ARM_GCC_BIN_DIR}/arm-none-eabi-g++${BIN_SUFFIX} )
SET(CMAKE_ASM_COMPILER ${ARM_GCC_BIN_DIR}/arm-none-eabi-gcc${BIN_SUFFIX} )

SET(CMAKE_OBJCOPY ${ARM_GCC_BIN_DIR}/arm-none-eabi-objcopy${BIN_SUFFIX} CACHE INTERNAL "objcopy tool")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
