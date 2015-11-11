cmake_minimum_required (VERSION 3.0)
project (Carma2)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

if(WIN32) 
add_definitions(/DWINVER=0x0600 /D_WIN32_WINNT=0x0600)
endif()

if (NOT CMAKE_BUILD_TYPE AND CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "No build type selected, default to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

option(DEVEL "DEVEL" OFF)
option(USE_64BIT_BUILD "USE_64BIT_BUILD" OFF)
option(USE_STATIC_LINKING "USE_STATIC_LINKING" ON)


if(CMAKE_COMPILER_IS_GNUCXX)
	SET(CMAKE_CXX_FLAGS "-std=c++11 -march=i686 -m32 -O2 -s -fPIC -fpermissive")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(ERROR "SUPPORT NOT COMPLETE")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /arch:SSE2 /Qpar-report:2")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /D _DEBUG /MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS} /MT /O1 /Ob1 /D NDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE       "${CMAKE_CXX_FLAGS} /MT /O2 /Ob2 /D NDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} /MT /Zi /O2 /Ob1 /D NDEBUG")
endif()

include_directories("src")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

set(GLOBAL_SOURCES ${GLOBAL_RC})


# Add extensions to build here
add_subdirectory(src)