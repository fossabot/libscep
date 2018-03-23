cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (libscep C)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###############################################################################
include (CheckCSourceCompiles)
include (CheckCSourceRuns)
include (CheckFunctionExists)
include (CheckIncludeFile)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckSymbolExists)
include (CheckTypeSize)

# Version
set (libscep_VERSION_MAJOR 0)
set (libscep_VERSION_MINOR 0)

if(LIB64)
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

set(INSTALL_LIB_DIR lib${LIBSUFFIX} CACHE PATH "Installation directory for libraries")
mark_as_advanced(INSTALL_LIB_DIR)

configure_file (
    "${PROJECT_SOURCE_DIR}/src/scep.h.in"
    "${PROJECT_BINARY_DIR}/scep.h"
    )

include_directories ("${PROJECT_BINARY_DIR}")
add_subdirectory (src)

# Unit tests
if( DEFINED NOTEST )
    message( STATUS "NOTE: Skipping tests" )
else ( DEFINED NOTEST )
set (TEST_TARGETS configuration util scep)
add_subdirectory (tests)
enable_testing ()
foreach (target ${TEST_TARGETS})
    add_test (NAME test_${target} COMMAND test_${target})
endforeach (target)
add_test (NAME test_message COMMAND test_message)
endif ( DEFINED NOTEST )
