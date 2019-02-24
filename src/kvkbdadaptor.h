/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -m -a kvkbdadaptor -i mainwidget.h -l MainWidget /home/cosmos/Softwares/Projects/CoreBox/kvkbd/src/org.corebox.kvkbd.Kvkbd.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef KVKBDADAPTOR_H_1550940326
#define KVKBDADAPTOR_H_1550940326

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include <QDBusAbstractAdaptor>

#include "mainwidget.h"

class QByteArray;

template<class T> class QList;
template<class Key, class Value> class QMap;

class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.corebox.kvkbd.Kvkbd
 */
class KvkbdAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.corebox.kvkbd.Kvkbd")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.corebox.kvkbd.Kvkbd\">\n"
"    <property access=\"read\" type=\"b\" name=\"alone\"/>\n"
"    <property access=\"readwrite\" type=\"b\" name=\"visible\"/>\n"
"    <property access=\"readwrite\" type=\"b\" name=\"locked\"/>\n"
"    <!--    <property name=\"autoResize\" type=\"b\" access=\"readwrite\"/>\n"
"    <method name=\"chooseFont\">\n"
"    </method>-->\n"
"  </interface>\n"
        "")
public:
    KvkbdAdaptor(MainWidget *parent);
    virtual ~KvkbdAdaptor();

    inline MainWidget *parent() const
    { return static_cast<MainWidget *>(QObject::parent()); }

public: // PROPERTIES
    Q_PROPERTY(bool alone READ alone)
    bool alone() const;

    Q_PROPERTY(bool locked READ locked WRITE setLocked)
    bool locked() const;
    void setLocked(bool value);

    Q_PROPERTY(bool visible READ visible WRITE setVisible)
    bool visible() const;
    void setVisible(bool value);

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
};

#endif
