project(botui)

option(wombat "Use the Wombat controller" ON)
option(DEBUG "Enable debug mode" OFF)
option(RELEASE "Enable release mode" OFF)
option(docker_cross "Cross compile option for docker container" OFF)
set(KIPR_VERSION_MAJOR 1)
set(KIPR_VERSION_MINOR 2)
set(KIPR_VERSION_PATCH 3)
cmake_minimum_required(VERSION 2.8.11)

#if (wallaby)
 # add_definitions(-DWALLABY)
  #set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/wallaby)
  #find_package(OpenCV REQUIRED)
#else()
 # set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/kovan)
  
#endif()

set(LIBKAR_ROOT ${CMAKE_SOURCE_DIR}/../libkar)
set(PCOMPILER_ROOT ${CMAKE_SOURCE_DIR}/../pcompiler)
set(LIBKIPR_ROOT ${CMAKE_SOURCE_DIR}/../libwallaby)


include_directories(/usr/local/include)

if(wombat)
	add_definitions(-DWOMBAT)
	set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/wombat)
endif()

if(docker_cross)
	link_directories("${LIBKAR_ROOT}/lib")
	link_directories("${PCOMPILER_ROOT}/lib")
	link_directories("${LIBKIPR_ROOT}/build")
	include_directories(/usr/local/include)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/deploy)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(INCLUDE ${CMAKE_SOURCE_DIR}/include/botui)
set(WOMBAT ${DEVICES}/kipr)
set(RC ${CMAKE_SOURCE_DIR}/rc)
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(UI ${CMAKE_SOURCE_DIR}/ui)
set(TS ${CMAKE_SOURCE_DIR}/ts)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${DBUS}/include
	${CMAKE_BINARY_DIR}
  ${DEVICE_DIR}/include
	${INCLUDE}
)


  
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Quick QuickWidgets QuickControls2)
 


find_package(OpenSSL REQUIRED)

file(GLOB INCLUDES ${INCLUDE}/*.h ${DEVICE_DIR}/include/*.h)
file(GLOB SOURCES ${SRC}/*.cpp ${DEVICE_DIR}/src/*.cpp)
file(GLOB UIS ${UI}/*)
file(GLOB TS_FILES ${TS}/*.ts)
file(GLOB DBUS_INTERFACES ${DBUS}/*.xml)

set(UI ${UIS})

set(QRC_FILES
	${RC}/icons.qrc
	${RC}/construction.qrc
	${RC}/qml.qrc
	${RC}/fonts.qrc
	${RC}/target.qrc
)


  
 

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(MOC_SRCS ${INCLUDES})
set(SRCS_CXX ${SOURCES})
qt6_wrap_cpp(SRCS_CXX ${MOC_SRCS} OPTIONS -DQT_VERSION=0x060000 -UQT_DBUS_LIB)
qt6_wrap_ui(SRCS_CXX ${UI})
qt6_add_resources(SRCS_CXX ${QRC_FILES})


add_definitions(-Wall)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(botui ${SRCS_CXX} ${QM_FILES} ${DBUS_INTERFACE_SOURCES})

if (DEBUG)
	add_definitions(-g)
endif()

if (RELEASE)
	add_definitions(-O3)
endif()

add_subdirectory(dbus)

target_link_libraries(botui
	Qt6::Core
	Qt6::Gui
	Qt6::Quick
	Qt6::QuickWidgets
	Qt6::Widgets
	Qt6::QuickControls2
	network_manager_dbus
	pcompiler
	z
	${OPENSSL_LIBRARIES}
)

IF(wombat)
  target_link_libraries(botui kipr)
ENDIF()

target_link_libraries(botui ${QT_LIBRARIES})


install(TARGETS botui DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION "/etc/botui/locale")

set(CPACK_PACKAGE_NAME botui)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR ${KIPR_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${KIPR_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${KIPR_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "KISS Institute for Practical Robotics")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
include(CPack)
