cmake_minimum_required(VERSION 3.10)
project(botui)

option(wombat "Use the Wombat controller" ON)
option(DEBUG "Enable debug mode" OFF)
option(RELEASE "Enable release mode" OFF)
option(docker_cross "Cross compile option for docker container" OFF)
set(KIPR_VERSION_MAJOR 2)
set(KIPR_VERSION_MINOR 0)
set(KIPR_VERSION_PATCH 1)

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


  
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Quick QuickWidgets QuickControls2 Xml)

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
	Qt6::Xml
	network_manager_dbus
	pcompiler
	z
	${OPENSSL_LIBRARIES}
)

IF(wombat)
  target_link_libraries(botui kipr)
ENDIF()

target_link_libraries(botui ${QT_LIBRARIES})

include(CTest)
if(BUILD_TESTING)
	find_package(Qt6 REQUIRED COMPONENTS Test)
	qt6_add_resources(BOTUI_UI_TEST_RESOURCES ${RC}/target.qrc)
	qt6_wrap_cpp(BOTUI_UI_TEST_MOCS
		include/botui/RootController.h
		include/botui/RootWindow.h
		include/botui/InputProviderWidget.h
		include/botui/DeclarativeView.h
	)
	add_executable(botui-ui-tests
		tests/RootWindowTest.cpp
		src/Calibrate.cpp
		src/RootController.cpp
		src/RootWindow.cpp
		src/InputProviderWidget.cpp
		src/DeclarativeView.cpp
		src/TouchCalibration.cpp
		include/botui/Calibrate.h
		include/botui/RootController.h
		include/botui/RootWindow.h
		include/botui/InputProviderWidget.h
		include/botui/DeclarativeView.h
		include/botui/TouchCalibration.h
		${BOTUI_UI_TEST_RESOURCES}
		${BOTUI_UI_TEST_MOCS}
	)
	target_include_directories(botui-ui-tests PRIVATE ${INCLUDE})
	target_link_libraries(botui-ui-tests PRIVATE
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Quick
		Qt6::QuickWidgets
		Qt6::Xml
		Qt6::Test
	)
	add_test(NAME botui-ui-tests COMMAND botui-ui-tests)
	set_tests_properties(botui-ui-tests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

	add_executable(botui-calibration-tests
		tests/TouchCalibrationTest.cpp
		src/TouchCalibration.cpp
		include/botui/TouchCalibration.h
	)
	target_include_directories(botui-calibration-tests PRIVATE ${INCLUDE})
	target_link_libraries(botui-calibration-tests PRIVATE
		Qt6::Core
		Qt6::Xml
		Qt6::Test
	)
	add_test(NAME botui-calibration-tests COMMAND botui-calibration-tests)

	add_executable(botui-orientation-tests
		tests/ScreenOrientationTest.cpp
		src/ScreenOrientation.cpp
		include/botui/ScreenOrientation.h
	)
	target_include_directories(botui-orientation-tests PRIVATE ${INCLUDE})
	target_link_libraries(botui-orientation-tests PRIVATE Qt6::Core Qt6::Test)
	add_test(NAME botui-orientation-tests COMMAND botui-orientation-tests)

	add_executable(botui-kanshi-process-tests
		tests/KanshiProcessTest.cpp
		src/KanshiProcess.cpp
		include/botui/KanshiProcess.h
	)
	target_include_directories(botui-kanshi-process-tests PRIVATE ${INCLUDE})
	target_link_libraries(botui-kanshi-process-tests PRIVATE Qt6::Core Qt6::Test)
	add_test(NAME botui-kanshi-process-tests COMMAND botui-kanshi-process-tests)
endif()


install(TARGETS botui DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION "/etc/botui/locale")
install(FILES botui.desktop DESTINATION share/applications)

set(CPACK_PACKAGE_NAME botui)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR ${KIPR_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${KIPR_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${KIPR_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "KISS Institute for Practical Robotics")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
#set (CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6quick6, libqt6core6t64, libqt6dbus6, libqt6network6, libqt6qml6, libqt6gui6, libqt6widgets6, libqt6quickwidgets6, libssl3t64, kipr, pcompiler, libkar, libc6, libgcc-s1, libstdc++6")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "pcompiler"
    "kipr"
	"qt6-wayland"
	"wlr-randr"
    # Add QML/plugin packages that are dynamically loaded at runtime.
)
include(CPack)

add_custom_target(package-debian
	COMMAND "${CMAKE_CPACK_COMMAND}"
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	DEPENDS botui
	COMMENT "Building Debian package with CPack"
	VERBATIM
)
