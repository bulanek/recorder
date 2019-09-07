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
set(DOXYGEN_PROJECT_NUMBER "FreeRTOS_0.1.1")
project( FreeRTOS VERSION 0.1.1 )

SET( LANGUAGE CXX)
enable_language(CXX)

# Build type
if (NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "Build type")
	message(STATUS "duild in default Release mode")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	add_definitions(-DcSIZE_BUILD)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_definitions(-DcDEBUG_BUILD)
else()
	message(SEND_ERROR "Wrong variable CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# Output verbosity (default is OFF)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set( CMAKE_VERBOSE_MAKEFILE "OFF" )
endif()

# Binary file name
set(APP_BIN_NAME recorder CACHE STRING "Name of application bin file")

# Source code root directory
set( SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Source)
get_filename_component( SRC_ROOT_ABS ${SRC_ROOT} ABSOLUTE )


set( FLAGS_ALL 	"-Wall \
		-pedantic \
		-Wextra \
		-fstrict-volatile-bitfields \
		-ffunction-sections \
		-fdata-sections \
		-fmessage-length=0 \
		-std=c11\
		-fdiagnostics-color=auto \
        -Werror=parentheses\
        -Werror=switch \
        -Werror=uninitialized \
        -Werror=maybe-uninitialized \
        -Werror=nonnull \
        -Werror=init-self \
        -Werror=sequence-point \
        -Werror=overflow \
        -Werror=unused-but-set-parameter \
        -Werror=bool-compare \
        -Werror=type-limits \
        -Werror=logical-not-parentheses \
        -Werror=override-init"
)

# Additional flags for eipd and acdd and support modules (apma, eipd_pb). Not applicable for other components.
set( FLAGS_EXTRA " -Werror=switch-enum \
        -Werror=switch-default \
        -Werror=unused-but-set-variable \
        -Werror=shadow \
        -Werror=bad-function-cast \
        -Werror=sign-compare \
        -Werror=sign-conversion \
        -Werror=parentheses\
        -Werror=implicit-function-declaration \
        -Werror=missing-field-initializers"
)
add_compile_options(
        -mcpu=cortex-m4
		-fstrict-volatile-bitfields
		-ffunction-sections
		-fdata-sections 
		-fmessage-length=0
		-std=c11
		-fdiagnostics-color=auto
        -ffreestanding
#        -Werror=switch
#        -Werror=uninitialized
#        -Werror=maybe-uninitialized
#        -Werror=nonnull
#        -Werror=init-self
#        -Werror=sequence-point
#        -Werror=overflow
#        -Werror=unused-but-set-parameter
#        -Werror=bool-compare
#        -Werror=type-limits
#        -Werror=logical-not-parentheses
#        -Werror=override-init
)


file(GLOB_RECURSE TMP_SOURCES_OWN ${SRC_ROOT}/${module}/*(.c|.cpp))

set(OPTIMIZATION_DEBUG "-O0")
set(OPTIMIZATION_RELEASE "-O2 -flto")

set(CMAKE_C_FLAGS_DEBUG " ${OPTIMIZATION_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE " ${OPTIMIZATION_RELEASE} ")
set(CMAKE_CXX_FLAGS_DEBUG " ${OPTIMIZATION_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE " ${OPTIMIZATION_RELEASE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-extensions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-use-cxa-atexit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage,-Map,${PROJECT_NAME}.map")

# ------------------------------------------------------------------------------
# SOURCES
# ------------------------------------------------------------------------------

# list all sources in all subdirectories.

set(SOURCES
    ${SRC_ROOT}/croutine.c
    ${SRC_ROOT}/event_groups.c
    ${SRC_ROOT}/list.c
    ${SRC_ROOT}/queue.c
    ${SRC_ROOT}/stream_buffer.c
    ${SRC_ROOT}/tasks.c
    ${SRC_ROOT}/timers.c
    ${SRC_ROOT}/portable/GCC/ARM_CM4F/port.c
    )

# ------------------------------------------------------------------------------
# INCLUDES
# ------------------------------------------------------------------------------
SET (INC_DIRS
	${SRC_ROOT}/..
	${SRC_ROOT}/include
    ${SRC_ROOT}/portable/GCC/ARM_CM4F/
    )

include_directories(${INC_DIRS})

# ------------------------------------------------------------------------------
# BUILD
# ------------------------------------------------------------------------------
add_library(${PROJECT_NAME} ${SOURCES})

# CMAKE_OBJCOPY defined in the Toolchain.
#add_custom_command(
#		TARGET ${PROJECT_NAME} 
#		POST_BUILD
#		COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME} ${APP_BIN_NAME}.bin
#		COMMENT "Creation of ${APP_BIN_NAME}.bin "
#		)

# ------------------------------------------------------------------------------
# DOC 
# ------------------------------------------------------------------------------

find_package(Doxygen REQUIRED dot)
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
        doc                                                 # custom target doc
        ${DOX_INPUT}
        COMMENT    "Generate man pages" 
)

# ------------------------------------------------------------------------------
# INSTALL
# ------------------------------------------------------------------------------
install(
	TARGETS ${PROJECT_NAME} 
    DESTINATION	${CMAKE_INSTALL_PREFIX}
)

