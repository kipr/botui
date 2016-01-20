project(botui)

option(qt5 "Use Qt5 instead of Qt4" OFF)
option(wallaby "Use the Wallaby controller" ON)

if (wallaby)
    add_definitions(-DWALLABY)
endif()

if(NOT qt5)
  cmake_minimum_required(VERSION 2.6.0)
else()
  cmake_minimum_required(VERSION 2.8.11)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/deploy)

set(INCLUDE ${CMAKE_SOURCE_DIR}/include/botui)
set(RC ${CMAKE_SOURCE_DIR}/rc)
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(UI ${CMAKE_SOURCE_DIR}/ui)

set(DEVICES ${CMAKE_SOURCE_DIR}/devices)
set(KOVAN ${DEVICES}/kovan)
set(SIMULATOR ${DEVICES}/simulator)

set(DBUS ${CMAKE_SOURCE_DIR}/dbus)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${DBUS}/include
	${CMAKE_BINARY_DIR}
	${KOVAN}/include
	${SIMULATOR}/include
	${INCLUDE}
)

if(NOT qt5)
  cmake_minimum_required(VERSION 2.6.0)
  find_package(Qt4 REQUIRED)
  set(QT_USE_QTDECLARATIVE TRUE)
  set(QT_USE_QTNETWORK TRUE)
  set(QT_USE_QTDBUS TRUE)

  include(${QT_USE_FILE})
else()
  
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Declarative REQUIRED)
  find_package(Qt5Network REQUIRED)
endif()

file(GLOB INCLUDES ${INCLUDE}/*.h ${KOVAN}/include/*.h ${SIMULATOR}/include/*.h)
file(GLOB SOURCES ${SRC}/*.cpp ${KOVAN}/src/*.cpp ${SIMULATOR}/src/*.cpp)
file(GLOB UIS ${UI}/*)


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
else()
  # set(CMAKE_AUTOMOC ON)
  set(MOC_SRCS ${INCLUDES})
  set(SRCS_CXX ${SOURCES})
  qt5_wrap_cpp(SRCS_CXX ${MOC_SRCS} OPTIONS -DQT_VERSION=0x050000 -UQT_DBUS_LIB)
  qt5_wrap_ui(SRCS_CXX ${UI})
  qt5_add_resources(SRCS_CXX ${QRC_FILES})
endif()

SET(CMAKE_CXX_FLAGS "-Wall")

IF(APPLE)
  add_definitions(-g)
	link_directories(/Library/Frameworks/ /usr/local/lib)
  include_directories(/usr/local/include)
ELSEIF(WIN32)
	set(CMAKE_CXX_FLAGS "-Wl,--enable-auto-import")
ENDIF()

if(WIN32)
	add_executable(botui WIN32 ${SRCS_CXX})
else()
  add_executable(botui ${SRCS_CXX})
endif()

add_definitions(-O3)

target_link_libraries(botui wallaby pcompiler kar z opencv_core opencv_highgui opencv_imgproc)

if(NOT qt5)
  target_link_libraries(botui ${QT_LIBRARIES})
else()
  qt5_use_modules(botui Widgets Declarative Network)
endif()
