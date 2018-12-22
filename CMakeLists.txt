cmake_minimum_required(VERSION 3.1.0)

project(wally C CXX)

add_subdirectory(libexif)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

set(wally_SRCS
  src/application.cpp
  src/engine.cpp
  src/exif.cpp
  src/image.cpp
  src/main.cpp
  src/settingswindow.cpp
  src/trayicon.cpp
  src/utils.cpp

  src/engines/deviantart.cpp
  src/engines/flickr.cpp
  src/engines/imgur.cpp
  src/engines/ipernity.cpp
)

IF(WIN32)
  add_executable(wally
    WIN32
    ${wally_SRCS}
    $<TARGET_OBJECTS:exif_objlib>
  )
ELSE()
  add_executable(wally
    ${wally_SRCS}
    $<TARGET_OBJECTS:exif_objlib>
  )
ENDIF()

target_compile_definitions(wally PRIVATE WALLY_VERSION="3.0.0")
target_compile_options(wally PRIVATE -Wall -Wextra -pedantic -std=c++14)
target_include_directories(wally PRIVATE libexif src)
target_link_libraries(wally Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
