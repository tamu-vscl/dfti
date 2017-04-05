cmake_minimum_required(VERSION 2.8.12)
project(dfti)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH ${dfti_SOURCE_DIR}/cmake_modules)

if(POLICY CMP0043)
cmake_policy(SET CMP0043 OLD)
endif()

if(POLICY CMP0048)
cmake_policy(SET CMP0048 OLD)
endif()

################################################################################
# Version
################################################################################
set(dfti_MAJOR_VERSION 2)
set(dfti_MINOR_VERSION 1)
set(dfti_PATCH_VERSION 0)
set(dfti_VERSION ${dfti_MAJOR_VERSION}.${dfti_MINOR_VERSION}.${dfti_PATCH_VERSION})

################################################################################
# Set the output directory for the build executables and libraries
################################################################################
set(dfti_RUNTIME_OUTPUT_DIRECTORY ${dfti_SOURCE_DIR}/bin CACHE PATH "Target for the binaries")
if(WIN32)
  set(dfti_LIBRARY_OUTPUT_DIRECTORY ${dfti_SOURCE_DIR}/bin CACHE PATH "Target for the libraries")
else()
  set(dfti_LIBRARY_OUTPUT_DIRECTORY ${dfti_SOURCE_DIR}/lib CACHE PATH "Target for the libraries")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${dfti_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${dfti_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dfti_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_INSTALL_PREFIX /usr)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set library path based on arch.
string(COMPARE EQUAL "arm" ${CMAKE_SYSTEM_PROCESSOR} _cmp)
if(_cmp)
  set(dfti_TARGET_LIB_DIRECTORY "lib")
else()
  set(dfti_TARGET_LIB_DIRECTORY "lib64")
endif()

################################################################################
# Set the output directory for the build executables and libraries
################################################################################
set(CPACK_PACKAGE_VERSION ${dfti_VERSION})
set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_PACKAGE_NAME "dfti")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Joshua Harris <joshua.a.harris@tamu.edu>")
set(CPACK_PACKAGE_VENDOR "TAMU Vehicle Systems & Control Laboratory")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
# Vars for DEB/RPM fields.
set(DFTI_DESCRIPTION "Developmental flight test instrumentation software.")
set(DFTI_LICENSE "ISC License.")
set(DFTI_URL "https://github.tamu.edu/vscl/dfti")
# DEB variables.
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${DFTI_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${DFTI_URL})
# RPM variables.
set(CPACK_RPM_PACKAGE_DESCRIPTION ${DFTI_DESCRIPTION})
set(CPACK_RPM_PACKAGE_LICENSE ${DFTI_LICENSE})
set(CPACK_RPM_PACKAGE_URL ${DFTI_URL})
include(CPack)


################################################################################
# Detect OS and define macros appropriately
################################################################################
if(WIN32)
  add_definitions(-DWINDOWS)
  message(STATUS "Compiling on Windows")
  if(MSVC)
    message(STATUS "Compiling with MSVC")
    add_definitions(-DNOMINMAX)
    add_definitions(-D_USE_MATH_DEFINES)
    # SSE2 optimizations
    add_definitions("/arch:SSE2")
    if(BUILD_SHARED_LIBS)
      # disable warning on missing DLL interfaces
      add_definitions("/wd4251")
    endif()
  endif()
elseif(CYGWIN)
  message(STATUS "Compiling on Cygwin")
  add_definitions(-DCYGWIN)
elseif(APPLE)
  add_definitions(-DUNIX)
  message(STATUS "Compiling on OSX")
elseif(UNIX)
  add_definitions(-DUNIX)
  message(STATUS "Compiling on Unix")
endif()

# Define macros to pass CMake version to code.
add_definitions(-DDFTI_VERSION="${dfti_VERSION}")
add_definitions(-DDFTI_MAJOR_VERSION="${dfti_MAJOR_VERSION}")
add_definitions(-DDFTI_MINOR_VERSION="${dfti_MINOR_VERSION}")
add_definitions(-DDFTI_PATCH_VERSION="${dfti_PATCH_VERSION}")

################################################################################
# C++11 support
#
# Notes:
# OS X 10.8 Mountain Lion and above default to using the clang compiler,
# which does not use C++ 11 unless explicitly told to, as below.
################################################################################
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -O2")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
  endif()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} supports C++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++ -O2")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O2")
  endif()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} supports C++0X")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


################################################################################
# Add a couple custom targets to quickly make a debug or release version
################################################################################
set(CMAKE_BUILD_TYPE Release)

add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
)

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)


################################################################################
# Project Dependencies:
################################################################################
# Qt5 via Homebrew on OS X requires some fancy PREFIX_PATH changes
if(APPLE)
  set(CMAKE_PREFIX_PATH /usr/local/opt/qt5)
elseif(UNIX)
  set(CMAKE_PREFIX_PATH /opt/qt5/5.5/gcc_64)
endif()
find_package(Qt5Core       REQUIRED)
find_package(Qt5Network    REQUIRED)
find_package(Qt5SerialPort REQUIRED)


################################################################################
# Variables
################################################################################
set(LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)

link_directories(
  ${LIB_DIR}
)

include_directories(
  ${HEADER_DIR}
  ${EXTERN_DIR}
)


################################################################################
# Build the project and local dependencies
################################################################################
set(CMAKE_AUTOMOC ON)
add_subdirectory(src)
set(CMAKE_AUTOMOC OFF)
