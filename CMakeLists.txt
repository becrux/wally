cmake_minimum_required(VERSION 3.1.0)

project(wally C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -g3 -ggdb -gdwarf-4")
set(CMAKE_C_FLAGS_RELEASE "-O3 -flto")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -g3 -ggdb -gdwarf-4")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")

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

set_property(TARGET wally PROPERTY CXX_STANDARD 14)

target_compile_definitions(wally
  PRIVATE
    WALLY_VERSION="3.0.0"
    QT_DEPRECATED_WARNINGS
)

IF(WIN32)
  target_compile_definitions(wally
    PRIVATE
      WIN32
  )
ELSEIF(APPLE)
  target_compile_definitions(wally
    PRIVATE
      MACOSX
  )
ELSE()
  target_compile_definitions(wally
    PRIVATE
      LINUX
  )
ENDIF()

target_compile_options(wally
  PRIVATE
    -Wall
    -Wextra
    -pedantic

    -Wcast-qual
    -Wcatch-value
    -Wclass-memaccess
    -Wconversion
    -Wctor-dtor-privacy
    -Wdate-time
    -Wdelete-non-virtual-dtor
    -Wduplicated-branches
    -Wduplicated-cond
    -Wfloat-conversion
    -Wfloat-equal
    -Winit-self
    -Wlogical-op
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wnoexcept
    -Wnoexcept-type
    -Wnon-virtual-dtor
    -Wnull-dereference
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wredundant-decls
    -Wreorder
    -Wshadow
    -Wsign-conversion
    -Wsign-promo
    -Wsuggest-attribute=pure
    -Wsuggest-attribute=const
    -Wsuggest-override
    -Wunused-macros
    -Wuseless-cast
    -Wwrite-strings
    -Wzero-as-null-pointer-constant
)

target_include_directories(wally
  PRIVATE
    src
)

target_include_directories(wally
  SYSTEM PRIVATE
    libexif
)

target_link_libraries(wally
  PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)
