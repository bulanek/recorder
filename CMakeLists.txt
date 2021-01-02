# ==============================================================================
# README
# ==============================================================================
# Usage: cmake -G<Type of Makefile> -DCMAKE_TOOLCHAIN_PATH=<PATH> [path to this file]
#			-DCMAKE_BUILD_TYPE={Debug,Release}
# Note: Build folder should be outside of this directory (pnio_src) as test 
# sources are used for arm-gcc toolchain, which would be added 
# to compilation process (handled all of sources inside this directory)
#
# Default type of build is Release.
#
#	External vars:  None
# ------------------------------------------------------------------------------
# CMAKE PROJECT PROPERTIES
# ------------------------------------------------------------------------------

# CMake version
cmake_minimum_required( VERSION 3.6 )  # FILTER in list

# Project name
set(DOXYGEN_PROJECT_NUMBER "REC_0.1.1")
project( RECORDER VERSION 0.1.1 )

SET( LANGUAGE CXX ASM)
enable_language(CXX)
enable_language(ASM)

# Build type
if (NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Build type")
	message(STATUS "Build in default Debug mode")
endif()


# Output verbosity (default is OFF)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set( CMAKE_VERBOSE_MAKEFILE "OFF" )
endif()

# Binary file name
set(APP_BIN_NAME recorder CACHE STRING "Name of application bin file")

# Source code root directory
set( SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component( SRC_ROOT_ABS ${SRC_ROOT} ABSOLUTE )

add_definitions("-DTHUMB_INTERWORK")
add_definitions("-DSTM32")
add_definitions("-DSTM32F4")
add_definitions("-DSTM32F401xx")
add_definitions("-DSTM32F401RETx")
add_definitions("-DNUCLEO_F401RE")
if (${FF13_DMA} STREQUAL "ON")
add_definitions("-DFF13_DMA")
endif()
message(STATUS "FF13 write using DMA: ${FF13_DMA}")


add_compile_options(
        -mcpu=cortex-m4
		-mthumb
		-msoft-float
#		-mfloat-abi=hard
		-mfpu=fpv4-sp-d16
		-mfloat-abi=softfp
		-fstrict-volatile-bitfields
        -ffunction-sections
        -fdata-sections
        -fmessage-length=0
		-fdiagnostics-color=auto
        -ffreestanding
        -Werror=switch
        -Werror=uninitialized
        -Werror=maybe-uninitialized
		-pedantic 
		-Wextra 
		-fstrict-volatile-bitfields 
		-ffunction-sections 
		-fdata-sections 
		-fmessage-length=0 
        -fdiagnostics-show-option 
		-fdiagnostics-color=auto 
		-Werror=address 
		-Werror=cast-align 
        -Werror=return-type 
		-Werror=shift-count-negative 
		-Werror=shift-count-overflow 
		-Werror=shift-negative-value 
        -Werror=parentheses 
        -Werror=switch 
		-Werror=switch-bool 
		-Werror=switch-unreachable 
        -Werror=uninitialized 
        -Werror=maybe-uninitialized 
        -Werror=nonnull 
        -Werror=init-self 
        -Werror=sequence-point 
        -Werror=overflow 
        -Werror=unused-but-set-parameter 
		-Werror=stringop-overflow 
		-Werror=stringop-truncation 
        -Werror=bool-compare 
		-Werror=bool-operation 
#		-Werror=duplicated-branches 
		-Werror=duplicated-cond 
		-Werror=tautological-compare 
		-Werror=float-equal 
		-Werror=shadow 
		-Werror=dangling-else 
		-Werror=empty-body 
		-Werror=pointer-arith 
		-Werror=pointer-compare 
		-Werror=logical-op 
        -Werror=logical-not-parentheses 
		-Werror=missing-field-initializers 
		-Werror=missing-braces 
		-Werror=sizeof-pointer-div 
		-Werror=sizeof-pointer-memaccess 
		-Werror=sizeof-array-argument 
		-Werror=memset-elt-size 
		-Werror=memset-transposed-args 
		-Werror=volatile-register-var 
        -Werror=nonnull
		-Werror=stringop-overflow 
		-Werror=stringop-truncation
		-Werror=bool-operation 
        -Werror=init-self
        -Werror=sequence-point
        -Werror=overflow
        -Werror=unused-but-set-parameter
        -Werror=bool-compare
        -Werror=type-limits
        -Werror=logical-not-parentheses
        -Werror=enum-compare
		-Werror=empty-body 
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=missing-parameter-type")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit -Werror=implicit-int")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=nested-externs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=override-init")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=placement-new")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=extra-semi")

file(GLOB_RECURSE TMP_SOURCES_OWN ${SRC_ROOT}/${module}/*(.c|.cpp))

set(OPTIMIZATION_DEBUG "-O0")
set(OPTIMIZATION_RELEASE "-O2 ")

set(CMAKE_C_FLAGS_DEBUG " ${OPTIMIZATION_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE " ${OPTIMIZATION_RELEASE} -g  ")
set(CMAKE_CXX_FLAGS_DEBUG " ${OPTIMIZATION_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE " ${OPTIMIZATION_RELEASE} -g")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-extensions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-use-cxa-atexit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")

#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_CURRENT_BINARY_DIR}/FreeRTOS")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/arm-none-eabi/lib/thumb/v7/nofp")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_CURRENT_SOURCE_DIR}/STM32_Audio/Addons/PDM")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=cortex-m4")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mthumb")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${SRC_ROOT}/LinkerScript.ld")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=rdimon.specs")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections,-lm,-lc,-lPDMFilter_CM4F_GCC,--print-memory-usage,-Map,${PROJECT_NAME}.map")


# ------------------------------------------------------------------------------
# SOURCES
# ------------------------------------------------------------------------------

# startup

set(SOURCES ${SOURCES} ${SRC_ROOT}/startup/startup_stm32f401xe.s)

# list all et(SOURCES)
foreach(module src startup _cfg  )
	file(GLOB_RECURSE TMP_SOURCES ${SRC_ROOT}/${module}/*.c*)
	list(APPEND SOURCES ${TMP_SOURCES})
endforeach(module)
list(FILTER SOURCES EXCLUDE REGEX ".*/src/trace/app/*")

# Sources for FatFS
set (SOURCES ${SOURCES}
ff13/source/ff.c
ff13/source/ffsystem.c
ff13/source/ffunicode.c
)
#Sources for PDM Filter
set (SOURCES ${SOURCES}
	OpenPDMFilter/OpenPDMFilter.c
)


# ------------------------------------------------------------------------------
# INCLUDES
# ------------------------------------------------------------------------------
SET (INC_DIRS ${INC_DIRS}
	${ARM_GCC_INC_DIR} # gcc includes
	${ARM_GCC_INC_DIR}/c++/*/
	${SRC_ROOT}/CMSIS/core		
 	${SRC_ROOT}/CMSIS/device
  	${SRC_ROOT}/HAL_Driver/Inc
  	${SRC_ROOT}/STM32_Audio/Addons/PDM
	${SRC_ROOT}/OpenPDMFilter
  	${SRC_ROOT}/ff13/source
  	${SRC_ROOT}/inc
    ${SRC_ROOT}
    ${SRC_ROOT}/FreeRTOS
    ${SRC_ROOT}/FreeRTOS/Source/include
    ${SRC_ROOT}/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${SRC_ROOT}/src/tskma/_com
    ${SRC_ROOT}/src/hal/_com
    ${SRC_ROOT}/src/i2s/_com
    ${SRC_ROOT}/src/nv/_com
    ${SRC_ROOT}/src/trace/src/_com
    ${SRC_ROOT}/src/pdmpcm/_com
    ${SRC_ROOT}/_cfg
    ${SRC_ROOT}/_cfg/ff13
	${SRC_ROOT}/STM32_Audio/Addons/PDM
    )

include_directories(${INC_DIRS})

# ------------------------------------------------------------------------------
# BUILD
# ------------------------------------------------------------------------------

#add_subdirectory(FreeRTOS "FreeRTOS")

set (SOURCES ${SOURCES}
    ${SRC_ROOT}/FreeRTOS/Source/croutine.c
    ${SRC_ROOT}/FreeRTOS/Source/event_groups.c
    ${SRC_ROOT}/FreeRTOS/Source/list.c
    ${SRC_ROOT}/FreeRTOS/Source/queue.c
    ${SRC_ROOT}/FreeRTOS/Source/stream_buffer.c
    ${SRC_ROOT}/FreeRTOS/Source/tasks.c
    ${SRC_ROOT}/FreeRTOS/Source/timers.c
	${SRC_ROOT}/FreeRTOS/Source/portable/MemMang/heap_4.c
	# configuration
    ${SRC_ROOT}/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
)

set(EXECUTABLE ${PROJECT_NAME}.exe)

add_executable(${EXECUTABLE} ${SOURCES})

# CMAKE_OBJCOPY defined in the Toolchain.
add_custom_command(
		TARGET ${EXECUTABLE} 
		POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE} ${APP_BIN_NAME}.bin
		COMMENT "Creation of ${APP_BIN_NAME}.bin "
		)

add_custom_command(
		TARGET ${EXECUTABLE} 
		POST_BUILD
		COMMAND  cmake.exe -E copy ${EXECUTABLE} ${PROJECT_NAME}
		COMMENT "Creation of ${PROJECT_NAME}"
		)


# ------------------------------------------------------------------------------
# DOC 
# ------------------------------------------------------------------------------

find_package(Doxygen)
message(STATUS "Doxygen version: ${DOXYGEN_VERSION}")

set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/doc")
set(DOXYGEN_FILE_PATTERNS "*.c;*.cc;*.cpp;*.h;*.hpp")

# The RECURSIVE tag can be used to specify whether or not subdirectories should
# be searched for input files as well.
# The default value is: NO.
set(DOXYGEN_RECURSIVE YES)
##If the ENABLE_PREPROCESSING tag is set to YES, doxygen will evaluate all C-preprocessor directives found in the sources and include files
set(DOXYGEN_ENABLE_PREPROCESSING NO)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${SRC_ROOT}/README.md)

set(DOX_INPUT ) 
# Only public interface doc for release.
set(DOX_INPUT ${SRC_ROOT})
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(DOXYGEN_EXTRACT_PRIVATE YES)
endif()

# Function from Doxygen package. It generates doxygen config file using DOXYGEN_<doxygen_var> variables
doxygen_add_docs(
        recorder_doc                                                 # custom target doc
        ${DOX_INPUT}
        COMMENT    "Generate man pages" 
)

# ------------------------------------------------------------------------------
# INSTALL
# ------------------------------------------------------------------------------
install(
	TARGETS ${EXECUTABLE} 
    RUNTIME	DESTINATION	${CMAKE_INSTALL_PREFIX}
)
# Files created by add_custom_command (objcopy,..)
install(FILES 			${CMAKE_CURRENT_BINARY_DIR}/${APP_BIN_NAME}.bin 
						${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map  
		DESTINATION 	${CMAKE_INSTALL_PREFIX}
)
