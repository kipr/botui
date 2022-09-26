project(botui)

option(qt6 "use Qt6 instead of Qt5" ON)

option(wombat "Use the Wombat controller" ON)

cmake_minimum_required(VERSION 2.8.11)

#if (wallaby)
 # add_definitions(-DWALLABY)
  #set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/wallaby)
  #find_package(OpenCV REQUIRED)
#else()
 # set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/kovan)
  
#endif()

if(wombat)
	add_definitions(-DWOMBAT)
	set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/devices/wombat)
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

set(DBUS ${CMAKE_SOURCE_DIR}/dbus)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${DBUS}/include
	${CMAKE_BINARY_DIR}
  ${DEVICE_DIR}/include
	${INCLUDE}
)


if(qt6)

  
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Quick QuickWidgets)
 
  #find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
  
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


  
 
if(qt6)
  # set(CMAKE_AUTOMOC ON)
  set(MOC_SRCS ${INCLUDES})
  set(SRCS_CXX ${SOURCES})
  qt6_wrap_cpp(SRCS_CXX ${MOC_SRCS} OPTIONS -DQT_VERSION=0x060000 -UQT_DBUS_LIB)
  qt6_wrap_ui(SRCS_CXX ${UI})
  qt6_add_resources(SRCS_CXX ${QRC_FILES})
  #qt6_add_translation(QM_FILES ${TS_FILES})
endif()

add_definitions(-Wall)

# C++11
# http://www.guyrutenberg.com/2014/01/05/enabling-c11-c0x-in-cmake/
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
  add_definitions(--std=c++17)
elseif(COMPILER_SUPPORTS_CXX11)
  
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


IF(qt6)
	target_link_libraries(botui Qt6::Core Qt6::Gui Qt6::Quick Qt6::QuickWidgets Qt6::Widgets)

ENDIF()
target_link_libraries(botui pcompiler z ${OPENSSL_LIBRARIES})


IF(wombat)
  target_link_libraries(botui kipr)
ENDIF()

if(qt6)
  target_link_libraries(botui ${QT_LIBRARIES})

endif()


install(TARGETS botui DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION "/etc/botui/locale")
