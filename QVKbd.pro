TEMPLATE = app
TARGET = qvkbd

VERSION = 1.0.0

INCLUDEPATH += . src

greaterThan( QT_MAJOR_VERSION, 4 ) {
	QT += widgets x11extras
}

LIBS += -lX11 -lXtst

# Input
HEADERS += src/QVirtualKeyboard.hpp
HEADERS += src/QVirtualButton.hpp

SOURCES += src/QVirtualKeyboard.cpp
SOURCES += src/QVirtualButton.cpp
SOURCES += src/Main.cpp

RESOURCES += resources.qrc

MOC_DIR 	= build/moc
OBJECTS_DIR = build/obj
RCC_DIR		= build/qrc
UI_DIR      = build/uic

CONFIG += silent

unix {
	isEmpty( PREFIX ) {
		PREFIX = /usr
	}
	BINDIR = $$PREFIX/bin

	INSTALLS += target
	target.path = $$BINDIR
}
