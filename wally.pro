QT += core gui network widgets

TARGET = wally
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -std=c++14 -Wall -Wextra -pedantic

INCLUDEPATH += src libexif

SOURCES += \
  src/main.cpp \
  src/engine.cpp \
  src/application.cpp \
  src/trayicon.cpp \
  src/settingswindow.cpp \
  src/exif.cpp \
  src/image.cpp \
  src/engines/flickr.cpp \
  src/engines/ipernity.cpp \
  src/engines/deviantart.cpp \
  src/engines/imgur.cpp \
  \
  libexif/exif-byte-order.c \
  libexif/exif-content.c \
  libexif/exif-data.c \
  libexif/exif-entry.c \
  libexif/exif-format.c \
  libexif/exif-ifd.c \
  libexif/exif-loader.c \
  libexif/exif-log.c \
  libexif/exif-mem.c \
  libexif/exif-mnote-data.c \
  libexif/exif-tag.c \
  libexif/exif-utils.c \
  \
  libexif/canon/exif-mnote-data-canon.c \
  libexif/canon/mnote-canon-entry.c \
  libexif/canon/mnote-canon-tag.c \
  \
  libexif/fuji/exif-mnote-data-fuji.c \
  libexif/fuji/mnote-fuji-entry.c \
  libexif/fuji/mnote-fuji-tag.c \
  \
  libexif/olympus/exif-mnote-data-olympus.c \
  libexif/olympus/mnote-olympus-entry.c \
  libexif/olympus/mnote-olympus-tag.c \
  \
  libexif/pentax/exif-mnote-data-pentax.c \
  libexif/pentax/mnote-pentax-entry.c \
  libexif/pentax/mnote-pentax-tag.c

HEADERS += \
  src/engine.hpp \
  src/application.hpp \
  src/trayicon.hpp \
  src/settingswindow.hpp \
  src/exif.hpp \
  src/image.hpp \
  src/wm.hpp \
  src/engines/flickr.hpp \
  src/engines/ipernity.hpp \
  src/engines/deviantart.hpp \
  src/engines/imgur.hpp

linux-g++ {
  SOURCES += \
    src/wms/unix/shell.cpp \
    src/wms/unix/kde.cpp \
    src/wms/unix/kde3.cpp \
    src/wms/unix/gnome.cpp \
    src/wms/unix/gnomeshell.cpp \
    src/wms/unix/xfce.cpp \
    src/wms/unix/fluxbox.cpp \
    src/wms/unix/fvwm.cpp \
    src/wms/unix/blackbox.cpp \
    src/wms/unix/windowmaker.cpp

  HEADERS += \
    src/wms/unix/shell.hpp \
    src/wms/unix/kde.hpp \
    src/wms/unix/kde3.hpp \
    src/wms/unix/gnome.hpp \
    src/wms/unix/gnomeshell.hpp \
    src/wms/unix/xfce.hpp \
    src/wms/unix/fluxbox.hpp \
    src/wms/unix/fvwm.hpp \
    src/wms/unix/blackbox.hpp \
    src/wms/unix/windowmaker.hpp
}

win32 {
  SOURCES += \
    src/wms/win32/powershell.cpp \
    src/wms/win32/native.cpp

  HEADERS += \
    src/wms/win32/powershell.hpp \
    src/wms/win32/native.hpp
}

macx {
  SOURCES += \
    src/wms/macosx/shell.cpp \
    src/wms/macosx/native.cpp

  HEADERS += \
    src/wms/macosx/shell.hpp \
    src/wms/macosx/native.hpp
}

os2 {
  SOURCES += \
    src/wms/os2/native.cpp

  HEADERS += \
    src/wms/os2/native.hpp
}
