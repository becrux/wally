#-------------------------------------------------
#
# Project created by QtCreator 2018-09-18T20:22:47
#
#-------------------------------------------------

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

INCLUDEPATH += src

SOURCES += \
  src/main.cpp \
  src/engine.cpp \
  src/application.cpp \
  src/trayicon.cpp \
  src/settingswindow.cpp \
  src/engines/flickr.cpp \
    src/engines/ipernity.cpp \
    src/engines/deviantart.cpp

HEADERS += \
  src/engine.hpp \
  src/application.hpp \
  src/trayicon.hpp \
  src/settingswindow.hpp \
  src/engines/flickr.hpp \
    src/engines/ipernity.hpp \
    src/engines/deviantart.hpp
