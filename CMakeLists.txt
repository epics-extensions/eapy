cmake_minimum_required(VERSION 2.6)

project(EAPY)

##################################################################
# version and installation path (defined after the scidb approach)
##################################################################

set(PACKAGE_NAME "eapy")

# set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/")
# set(CMAKE_INSTALL_PREFIX "/opt/eapy/")

##################################################################
# general 
##################################################################

include_directories("include")

##################################################################
# cmake
##################################################################

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
"${CMAKE_SOURCE_DIR}/cmake/Modules/")

##################################################################
# boost
##################################################################

find_package(Boost REQUIRED 
system program_options serialization regex filesystem thread
unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})

##################################################################
# swig
##################################################################

find_package(SWIG REQUIRED)
find_package(PythonLibs REQUIRED)

include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")
include_directories(${PYTHON_INCLUDE_DIRS})

EXECUTE_PROCESS(COMMAND python -c "import sys;print'%s.%s'%sys.version_info[0:2]" OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

MESSAGE("Python version: ${PYTHON_VERSION}")

SET(PYTHON_DEST "/usr/lib/python${PYTHON_VERSION}/dist-packages" )

##################################################################
# npy
##################################################################

find_package(NumPy REQUIRED)
include_directories(${NUMPY_INCLUDE_DIRS})

##################################################################
# xerces-c
##################################################################

find_package(XercesC REQUIRED)
include_directories(${XERCESC_INCLUDE_DIRS})

MESSAGE("XERCES-C library: ${XERCESC_LIBRARY}")

##################################################################
# epics v3
##################################################################

set(EPICS_BASE         "$ENV{EPICS_BASE}")
set(EPICS_HOST_ARCH    "$ENV{EPICS_HOST_ARCH}")
set(EPICS_LIB_PREFIX   "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}")
set(EPICS_LIB_COM      "${EPICS_LIB_PREFIX}/libCom.a")
set(EPICS_LIB_CA       "${EPICS_LIB_PREFIX}/libca.a")

include_directories("${EPICS_BASE}/include")
include_directories("${EPICS_BASE}/include/os/Linux")

##################################################################
# epics v4
##################################################################

set(EV4_BASE "$ENV{EV4_BASE}")

set(EPICS4_CPP_LIBPVMB  
"${EV4_BASE}/pvCommonCPP/lib/${EPICS_HOST_ARCH}/libpvMB.so")

set(EPICS4_CPP_LIBPVDATA    
"${EV4_BASE}/pvDataCPP/lib/${EPICS_HOST_ARCH}/libpvData.so")

set(EPICS4_CPP_LIBPVACCESS  
"${EV4_BASE}/pvAccessCPP/lib/${EPICS_HOST_ARCH}/libpvAccess.so")

include_directories("${EV4_BASE}/pvCommonCPP/include")
include_directories("${EV4_BASE}/pvDataCPP/include")
include_directories("${EV4_BASE}/pvAccessCPP/include")

##################################################################
# ea4
##################################################################

set(EA_CPP         "$ENV{EA_CPP}")

include_directories("${EA_CPP}/include/common")

set(EA_LIB_PREFIX   "${EA_CPP}/lib/${EPICS_HOST_ARCH}")
set(EA_LIB_COMMON   "${EA_LIB_PREFIX}/libea4Common.so")
# set(EA_LIB_CLASSIC  "${EA_LIB_PREFIX}/libea4Classic.so")

##################################################################
# eapy
##################################################################

include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory("src")
