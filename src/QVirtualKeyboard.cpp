/*
	*
	* This file is a part of QVKbd Project.
	* Copyright 2019 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 3 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program. If not, see <http://www.gnu.org/licenses/>.
	*
*/

// X11 Headers
#include <X11/X.h>
#include "fixx11h.h"

// Local Headers
#include "QVirtualKeyboard.hpp"

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QScreen>
#include <QX11Info>

#include <X11/extensions/XTest.h>
#include <X11/Xlocale.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>

#include <X11/XKBlib.h>

QVirtualKeyboard::QVirtualKeyboard( QWidget *parent ) : QWidget( parent ) {

	createKeyboard();
	setWidgetProperties();

	trayIcon = new QVKTrayIcon( this );
	connect( trayIcon, SIGNAL( toggleShowHide() ), this, SLOT( toggleShowHide() ) );
};

void QVirtualKeyboard::createKeyboard() {

	setFont( QFont( "sans", 7 ) );
	QSettings keymap( ":/resources/en_US.keymap", QSettings::NativeFormat );

	QVBoxLayout *keyboardLyt = new QVBoxLayout();
	keyboardLyt->setSpacing( 1 );

	Q_FOREACH( QString row, keymap.value( "Rows" ).toStringList() ) {

		keymap.beginGroup( row );

		QHBoxLayout *lyt = new QHBoxLayout();
		lyt->setContentsMargins( QMargins() );
		lyt->setSpacing( 1 );

		Q_FOREACH( QString key, keymap.value( "Order" ).toStringList() ) {

			QStringList keyNames = keymap.value( key ).toStringList();
			unsigned int keyCode = key.toInt();

			QVirtualButton *btn = new QVirtualButton( keyCode, keyNames, this );
			connect( btn, SIGNAL( sendKey( unsigned int ) ), this, SLOT( processKeyPress( unsigned int ) ) );
			connect( btn, SIGNAL( longPressed() ), this, SLOT( releaseLongPress() ) );

			if ( btn->isModifier() )
				modifiers << btn;

			else if ( keyCode == 999 )
				connect( btn, SIGNAL( clicked() ), qApp, SLOT( quit() ) );

			lyt->addWidget( btn );
		}

		keyboardLyt->addLayout( lyt );

		keymap.endGroup();
	}

	setLayout( keyboardLyt );
};

void QVirtualKeyboard::setWidgetProperties() {

	setWindowTitle( "QVirtualKeyboard" );
	setWindowIcon( QIcon( ":/resources/tray.png" ) );

    setAttribute( Qt::WA_ShowWithoutActivating );
    setWindowFlags( Qt::ToolTip | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );

	QSize kbdSize = QSize( DEFAULT_KEY_WIDTH * 17 + 18, DEFAULT_KEY_HEIGHT * 6 + 18 );
    QRect scrnSize = qApp->primaryScreen()->availableGeometry();
    int hpos = scrnSize.width() - kbdSize.width();
    int vpos = scrnSize.height() - kbdSize.height();

    move( hpos, vpos );
    setFixedSize( kbdSize );
};

void QVirtualKeyboard::sendKey( unsigned int keycode ) {

	Window currentFocus;
	int revertTo;

	Display *display = QX11Info::display();
	XGetInputFocus( display, &currentFocus, &revertTo );

	QListIterator<QVirtualButton *> itr( modifiers );
	while ( itr.hasNext() ) {
		QVirtualButton *mod = itr.next();
		if ( mod->isChecked() ) {
			XTestFakeKeyEvent( display, mod->keyCode(), true, 2 );
		}
	}

	XTestFakeKeyEvent( display, keycode, true, 2 );
	XTestFakeKeyEvent( display, keycode, false, 2 );

	itr.toFront();
	while ( itr.hasNext() ) {
		QVirtualButton *mod = itr.next();
		if ( mod->isChecked() )
			XTestFakeKeyEvent( display, mod->keyCode(), false, 2 );
	}

	XFlush( display );
};

void QVirtualKeyboard::mousePressEvent( QMouseEvent *mpEvent ) {

	gpress = mpEvent->globalPos();

	dragged = true;
	dragPoint = mpEvent->pos();
};

void QVirtualKeyboard::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( !dragged )
		return;

	moved = true;

	QPoint curr( mmEvent->globalPos().x() - dragPoint.x(), mmEvent->globalPos().y() - dragPoint.y() );
	move( curr );
};

void QVirtualKeyboard::mouseReleaseEvent( QMouseEvent * ) {

	dragged = false;
	moved = false;
};

void QVirtualKeyboard::processKeyPress( unsigned int keyCode ) {

	sendKey( keyCode );

	QListIterator<QVirtualButton *> itr( modifiers );
	while ( itr.hasNext() ) {
		QVirtualButton *mod = itr.next();
		if ( mod->isChecked() )
			mod->setChecked( false );
	}
};

void QVirtualKeyboard::toggleShowHide() {

	if ( isVisible() )
		hide();

	else
		show();
};

void QVirtualKeyboard::releaseLongPress() {

	QVirtualButton *btn = qobject_cast<QVirtualButton*>( sender() );
	unsigned int keyCode = btn->keyCode();

	/* Virtual pressing of Left Shift to simulate Shift + Key */
	QListIterator<QVirtualButton *> itr( modifiers );
	while ( itr.hasNext() ) {
		QVirtualButton *mod = itr.next();
		if ( mod->keyCode() == 50 )
			mod->setChecked( true );
	}

	processKeyPress( keyCode );
};
