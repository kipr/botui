project(botui)

option(qt5 "Use Qt5 instead of Qt4" OFF)
option(wallaby "Use the Wallaby controller" ON)

if(NOT qt5)
  cmake_minimum_required(VERSION 2.6.0)
else()
  cmake_minimum_required(VERSION 2.8.11)
endif()

if (wallaby)
  add_definitions(-DWALLABY)
  set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/wallaby)
else()
  set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/kovan)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/deploy)

set(INCLUDE ${CMAKE_SOURCE_DIR}/include/botui)
set(RC ${CMAKE_SOURCE_DIR}/rc)
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(UI ${CMAKE_SOURCE_DIR}/ui)
set(TS ${CMAKE_SOURCE_DIR}/ts)

set(DBUS ${CMAKE_SOURCE_DIR}/dbus)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${DBUS}/include
	${CMAKE_BINARY_DIR}
  ${DEVICE_DIR}/include
	${INCLUDE}
)

if(NOT qt5)
  cmake_minimum_required(VERSION 2.6.0)
  find_package(Qt4 REQUIRED)
  find_package( PythonInterp 2.7 REQUIRED )
  find_package( PythonLibs 2.7 REQUIRED )
  set(QT_USE_QTDECLARATIVE TRUE)
  set(QT_USE_QTNETWORK TRUE)
  set(QT_USE_QTDBUS TRUE)

  include(${QT_USE_FILE})
else()
  
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Declarative REQUIRED)
  find_package(Qt5Network REQUIRED)
endif()

find_package(OpenSSL REQUIRED)

file(GLOB INCLUDES ${INCLUDE}/*.h ${DEVICE_DIR}/include/*.h)
file(GLOB SOURCES ${SRC}/*.cpp ${DEVICE_DIR}/src/*.cpp)
file(GLOB UIS ${UI}/*)
file(GLOB TS_FILES ${TS}/*.ts)

set(UI ${UIS})

set(QRC_FILES
	${RC}/icons.qrc
	${RC}/construction.qrc
	${RC}/qml.qrc
	${RC}/fonts.qrc
	${RC}/target.qrc
)

if(NOT qt5)
  file(GLOB DBUS_INCLUDES ${DBUS}/include/*.h)
  file(GLOB DBUS_SOURCES ${DBUS}/src/*.cpp)
  set(INCLUDES ${INCLUDES} ${DBUS_INCLUDES})
  set(SOURCES ${SOURCES} ${DBUS_SOURCES})
  
  set(MOC_SRCS ${INCLUDES})
  set(SRCS_CXX ${SOURCES})
  
  QT4_WRAP_CPP(SRCS_CXX ${MOC_SRCS} OPTIONS -DQT_VERSION=0x040000)
  QT4_WRAP_UI(SRCS_CXX ${UI})
  QT4_ADD_RESOURCES(SRCS_CXX ${QRC_FILES})
  QT4_ADD_TRANSLATION(QM_FILES ${TS_FILES})
else()
  # set(CMAKE_AUTOMOC ON)
  set(MOC_SRCS ${INCLUDES})
  set(SRCS_CXX ${SOURCES})
  qt5_wrap_cpp(SRCS_CXX ${MOC_SRCS} OPTIONS -DQT_VERSION=0x050000 -UQT_DBUS_LIB)
  qt5_wrap_ui(SRCS_CXX ${UI})
  qt5_add_resources(SRCS_CXX ${QRC_FILES})
  qt5_add_translation(QM_FILES ${TS_FILES})
endif()

add_definitions(-Wall)

# C++11
# http://www.guyrutenberg.com/2014/01/05/enabling-c11-c0x-in-cmake/
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  add_definitions(--std=c++11)
elseif(COMPILER_SUPPORTS_CXX0X)
  add_definitions(--std=c++0x)
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

IF(APPLE)
  add_definitions(-g)
	link_directories(/Library/Frameworks/ /usr/local/lib)
  include_directories(/usr/local/include)
ELSEIF(WIN32)
	set(CMAKE_CXX_FLAGS "-Wl,--enable-auto-import")
ENDIF()

if(WIN32)
	add_executable(botui WIN32 ${SRCS_CXX} ${QM_FILES})
else()
  add_executable(botui ${SRCS_CXX} ${QM_FILES})
endif()

add_definitions(-O3)

target_link_libraries(botui pcompiler z opencv_core opencv_highgui opencv_imgproc ${OPENSSL_LIBRARIES})
IF(wallaby)
  target_link_libraries(botui wallaby)
ELSEIF(wombat)
  target_link_libraries(botui kipr)
ELSE()
  target_link_libraries(botui kovan)
ENDIF()

if(NOT qt5)
  target_link_libraries(botui ${QT_LIBRARIES})
else()
  qt5_use_modules(botui Widgets Declarative Network)
endif()


install(TARGETS botui DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION "/etc/botui/locale")
