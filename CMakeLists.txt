cmake_minimum_required(VERSION 3.1.0)

project(wally C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
if(MSVC)
  set(CMAKE_C_FLAGS_DEBUG "/MDd /Od")
  set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /GR")
  set(CMAKE_C_FLAGS_RELEASE "/MD /GL /Ox /Gw")
  set(CMAKE_CXX_FLAGS_RELEASE "/MD /GL /Ox /GR /Gw")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO /OPT:REF")
else()
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g -g3 -ggdb -gdwarf-4 -fstack-protector")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -flto")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -g3 -ggdb -gdwarf-4 -fstack-protector -frtti -fexceptions")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -frtti -fexceptions")
endif()

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

if(OS2)
  set(wally_SRCS ${wally_SRCS}
    src/wms/os2/native.cpp
  )

  set(wally_DEFINES OS2)
elseif(WIN32)
  set(wally_SRCS ${wally_SRCS}
    src/wms/win32/native.cpp
    src/wms/win32/powershell.cpp
  )

  set(wally_DEFINES WIN32)
  set(wally_APP_FLAG WIN32)
elseif(APPLE)
  set(wally_SRCS ${wally_SRCS}
    src/wms/macosx/native.cpp
    src/wms/macosx/shell.cpp
  )

  set(wally_DEFINES MACOSX)
else()
  set(wally_SRCS ${wally_SRCS}
    src/wms/unix/blackbox.cpp
    src/wms/unix/fluxbox.cpp
    src/wms/unix/fvwm.cpp
    src/wms/unix/gnome.cpp
    src/wms/unix/gnomeshell.cpp
    src/wms/unix/kde.cpp
    src/wms/unix/kde3.cpp
    src/wms/unix/shell.cpp
    src/wms/unix/windowmaker.cpp
    src/wms/unix/xfce.cpp
  )

  set(wally_DEFINES LINUX)
endif()

qt5_add_resources(wally_SRCS res/images.qrc)

add_executable(wally
  ${wally_APP_FLAG}
  ${wally_SRCS}
  $<TARGET_OBJECTS:exif_objlib>
)

set_property(TARGET wally PROPERTY CXX_STANDARD 14)

target_compile_definitions(wally
  PRIVATE
    WALLY_VERSION="3.0.0"
    QT_DEPRECATED_WARNINGS
    ${wally_DEFINES}
)

if(MSVC)
  target_compile_options(wally
    PRIVATE
      /W4
  )
else()
  target_compile_options(wally
    PRIVATE
      -std=c++14
      -fstrict-aliasing
      -fipa-pure-const

      -Wall
      -Wextra
      -pedantic
      -Werror

      -Wcast-qual
      #-Wcatch-value
      #-Wclass-memaccess
      -Wconversion
      -Wctor-dtor-privacy
      -Wdate-time
      -Wdelete-non-virtual-dtor
      -Wduplicated-branches
      -Wduplicated-cond
      #-Wextra-semi
      -Wfloat-conversion
      -Wfloat-equal
      -Winit-self
      -Wlogical-op
      -Wmissing-declarations
      -Wmissing-include-dirs
      -Wnoexcept
      -Wnon-virtual-dtor
      -Wnull-dereference
      -Wold-style-cast
      -Woverloaded-virtual
      -Wparentheses
      -Wpointer-arith
      -Wredundant-decls
      -Wreorder
      -Wshadow
      -Wsign-conversion
      -Wsign-promo
      #-Wsuggest-attribute=pure
      #-Wsuggest-attribute=const
      -Wsuggest-override
      -Wstack-protector
      -Wstrict-aliasing
      -Wstrict-overflow
      -Wswitch-default
      -Wuninitialized
      -Wunused-macros
      -Wunused-parameter
      -Wwrite-strings
      -Wzero-as-null-pointer-constant
  )
endif()

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
