cmake_minimum_required(VERSION 2.8.10)
project(ndi-streamer)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

if(WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set(${PROJECT_NAME}_PLATFORM_LIBS Dwmapi)
  add_definitions(-DNOMINMAX)
elseif(APPLE)
  find_package(Threads REQUIRED)
  find_library(corefoundation_lib CoreFoundation REQUIRED)
  find_library(cocoa_lib Cocoa REQUIRED)
  find_library(coremedia_lib CoreMedia REQUIRED)
  find_library(avfoundation_lib AVFoundation REQUIRED)
  find_library(coregraphics_lib CoreGraphics REQUIRED)
  find_library(corevideo_lib CoreVideo REQUIRED)

  set(${PROJECT_NAME}_PLATFORM_LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    ${corefoundation_lib}
    ${cocoa_lib}
    ${coremedia_lib}
    ${avfoundation_lib}
    ${coregraphics_lib}  
    ${corevideo_lib}
  ) 
else()
	find_package(X11 REQUIRED)
	if (!X11_XTest_FOUND)
 		message(FATAL_ERROR "X11 extensions are required, but not found!")
	endif()
	if (!X11_Xfixes_LIB)
 		message(FATAL_ERROR "X11 fixes extension is required, but not found!")
	endif()
	find_package(Threads REQUIRED)
	set(${PROJECT_NAME}_PLATFORM_LIBS
		${X11_LIBRARIES}
		${X11_Xfixes_LIB}
		${X11_XTest_LIB}
		${X11_Xinerama_LIB}
		${CMAKE_THREAD_LIBS_INIT}
	)
endif()

add_subdirectory(lib/screen_capture_lite EXCLUDE_FROM_ALL)

add_executable(
  ${PROJECT_NAME}
	app.cpp
)
target_link_libraries(${PROJECT_NAME} ndi screen_capture_lite ${${PROJECT_NAME}_PLATFORM_LIBS}) 
