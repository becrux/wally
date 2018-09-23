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
  src/engines/flickr.cpp \
  src/engines/ipernity.cpp \
  src/engines/deviantart.cpp \
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
  src/engines/flickr.hpp \
  src/engines/ipernity.hpp \
  src/engines/deviantart.hpp
