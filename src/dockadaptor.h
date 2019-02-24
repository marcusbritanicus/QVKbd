/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -m -a dockadaptor -i kbddock.h -l KbdDock /home/cosmos/Softwares/Projects/CoreBox/kvkbd/src/org.corebox.kvkbd.Dock.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DOCKADAPTOR_H_1550940326
#define DOCKADAPTOR_H_1550940326

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "kbddock.h"

class QByteArray;

template<class T> class QList;
template<class Key, class Value> class QMap;

class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.corebox.kvkbd.Dock
 */
class DockAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.corebox.kvkbd.Dock")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.corebox.kvkbd.Dock\">\n"
"    <property access=\"readwrite\" type=\"b\" name=\"visible\"/>\n"
"  </interface>\n"
        "")
public:
    DockAdaptor(KbdDock *parent);
    virtual ~DockAdaptor();

    inline KbdDock *parent() const
    { return static_cast<KbdDock *>(QObject::parent()); }

public: // PROPERTIES
    Q_PROPERTY(bool visible READ visible WRITE setVisible)
    bool visible() const;
    void setVisible(bool value);

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
};

#endif
