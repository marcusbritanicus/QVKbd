TEMPLATE = app

TARGET = qvkbd
INCLUDEPATH += .

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets x11extras
}

QT += xml dbus

LIBS += -lxslt -lX11 -lXtst

# Input
HEADERS += src/dragwidget.h
HEADERS += src/kbdtray.h
HEADERS += src/keysymconvert.h
HEADERS += src/kvkbdadaptor.h
HEADERS += src/kvkbdapp.h
HEADERS += src/mainwidget.h
HEADERS += src/resizabledragwidget.h
HEADERS += src/themeloader.h
HEADERS += src/vbutton.h
HEADERS += src/vkeyboard.h
HEADERS += src/x11keyboard.h

SOURCES += src/dragwidget.cpp
SOURCES += src/kbdtray.cpp
SOURCES += src/keysymconvert.cpp
SOURCES += src/kvkbdapp.cpp
SOURCES += src/kvkbdadaptor.cpp
SOURCES += src/main.cpp
SOURCES += src/mainwidget.cpp
SOURCES += src/resizabledragwidget.cpp
SOURCES += src/themeloader.cpp
SOURCES += src/vbutton.cpp
SOURCES += src/vkeyboard.cpp
SOURCES += src/x11keyboard.cpp

RESOURCES += resources.qrc

MOC_DIR 	= build/moc
OBJECTS_DIR = build/objs
RCC_DIR		= build/qrc
UI_DIR      = build/uic

CONFIG += silent
