TEMPLATE = app
TARGET = qvkbd

INCLUDEPATH += .

greaterThan( QT_MAJOR_VERSION, 4 ) {
	QT += widgets x11extras
}

QT += xml

LIBS += -lX11 -lXtst

# Input
HEADERS += src/keysymconvert.h
HEADERS += src/x11keyboard.h
HEADERS += src/CCVirtualKeyboard.hpp

SOURCES += src/keysymconvert.cpp
SOURCES += src/x11keyboard.cpp
SOURCES += src/CCVirtualKeyboard.cpp

RESOURCES += resources.qrc

MOC_DIR 	= build/moc
OBJECTS_DIR = build/obj
RCC_DIR		= build/qrc
UI_DIR      = build/uic

CONFIG += silent
