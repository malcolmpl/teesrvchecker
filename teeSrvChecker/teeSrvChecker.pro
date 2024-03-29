# -----------------------------------------------------------
# This file is generated by the Qt Visual Studio Integration.
# -----------------------------------------------------------
TEMPLATE = app
TARGET = teeSrvChecker
DESTDIR = ./debug
QT += network

win32 {
  CONFIG += windows
  RC_FILE = teeSrvChecker.rc
  LIBS += -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32 -lws2_32 -lqtmain
  DEFINES += WIN32
}

CONFIG += debug release
DEFINES += UNICODE QT_THREAD_SUPPORT QT_CORE_LIB QT_GUI_LIB
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/Debug \
    .
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

FORMS += teeabout.ui \
	teechecker.ui \
	teesettings.ui

SOURCES += main.cpp \
	gameserverchecker.cpp \
	masterchecker.cpp \
	teeabout.cpp \
	teechecker.cpp \
	teesettings.cpp \
	teetrayicon.cpp \
	teetreewidgetitem.cpp

HEADERS += common.h \
	gameserverchecker.h \
	masterchecker.h \
	teeabout.h \ 
	teechecker.h \
	teesettings.h \
	teetrayicon.h \
	teetreewidgetitem.h \
	versionno.h
