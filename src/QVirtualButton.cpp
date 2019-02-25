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

#include <X11/XKBlib.h>
#include "fixx11h.h"

// Local Headers
#include "QVirtualButton.hpp"

#include <QWidget>
#include <QTimer>
#include <QX11Info>

#define XK_Caps_Lock 0xffe5

const int DEFAULT_KEY_WIDTH = 36;
const int DEFAULT_KEY_HEIGHT = 36;

QMap<unsigned int, QSizeF> QVirtualButton::SizeKeyMap = QMap<unsigned int, QSizeF>();
QMap<unsigned int, QColor> QVirtualButton::ColorKeyMap = QMap<unsigned int, QColor>();

QList<unsigned int> QVirtualButton::modifiers = QList<unsigned int>() << 50 << 62 << 37 << 133 << 64 << 108 << 135 << 109;
bool QVirtualButton::mInitDone = false;

QVirtualButton::QVirtualButton( unsigned int code, QString name, QWidget *parent ) : QLabel( parent ) {

	/* Flags and state init */
	mModifier = false;
	mChecked = false;
	mCheckable = false;
	mColor = QColor( Qt::black );

	/* X11 KeyCode */
	mKeyCode = code;

	/* Init colos and sizes */
	initStaticMembers();

	/* Set Size */
	QSizeF ratio = SizeKeyMap.value( code, QSizeF() );
	if ( ratio.isValid() ) {
		if ( code == 888 )
			setFixedWidth( ( int )DEFAULT_KEY_WIDTH * ratio.width() );

		else
			setMinimumWidth( ( int )DEFAULT_KEY_WIDTH * ratio.width() );

		setFixedHeight( ( int )DEFAULT_KEY_HEIGHT * ratio.height() );
	}

	else {
		setFixedWidth( DEFAULT_KEY_WIDTH );
		setFixedHeight( DEFAULT_KEY_HEIGHT );
	}

	/* Set Color */
	mColor = ColorKeyMap.value( code, QColor( 0, 0, 0 ) );
	setStyleSheet( QString( "color: %1; border: 0.5px solid %1; border-radius: 2px;" ).arg( mColor.name() ) );

	/* Name of the key */
	mName = QString( name );
	setText( mName );
	setAlignment( Qt::AlignCenter );

	/* Enable checked state for modifiers */
	if ( modifiers.contains( mKeyCode ) ) {
		mModifier = true;
		setCheckable( true );
		setChecked( false );
	}

	/* Enable mouse tracking */
	setMouseTracking( true );

	/* If this is caps lock, keep checking for physical keyboard caps */
	if ( mKeyCode == 66 ) {
		/* Set initial caps lock state */
		setCapsLockState();

		QTimer *timer = new QTimer( this );
		timer->setInterval( 200 );
		connect( timer, SIGNAL( timeout() ), this, SLOT( setCapsLockState() ) );

		timer->start();
	}
};

unsigned int QVirtualButton::keyCode() {

	return mKeyCode;
};

bool QVirtualButton::isModifier() {

	return mModifier;
};

void QVirtualButton::setModifier( bool state ) {

	mModifier = state;
};

void QVirtualButton::setCheckable( bool state ) {

	mCheckable = state;
	if ( not mCheckable )
		mChecked = false;
};

bool QVirtualButton::isChecked() {

	return mChecked;
};

void QVirtualButton::setChecked( bool state ) {

	mChecked = state;
	if ( mChecked ) {
		setStyleSheet( QString(
				"color: %1; border: 0.5px solid %1; border-radius: 2px; background-color: rgba(%2, %3, %4, 100);"
			).arg( mColor.name() ).arg( mColor.red() ).arg( mColor.green() ).arg( mColor.blue() )
		);
	}

	else
		setStyleSheet( QString( "color: %1; border: 0.5px solid %1; border-radius: 2px;" ).arg( mColor.name() ) );
};

void QVirtualButton::setCapsLockState() {

	KeySym       iKey = XK_Caps_Lock;
	int          iKeyMask = 0;
	Window       wDummy1, wDummy2;
	int          iDummy3, iDummy4, iDummy5, iDummy6;
	unsigned int iMask;

	Display* display = QX11Info::display();

	XModifierKeymap* map = XGetModifierMapping( display );
	KeyCode keyCode = XKeysymToKeycode( display, iKey );

	if ( keyCode == NoSymbol ) {
		setChecked( false );
		return;
	}

	else {
		for ( int i = 0; i < 8; ++i ) {
			if ( map->modifiermap[ map->max_keypermod * i ] == keyCode ) {
				iKeyMask = 1 << i;
			}
		}

		XQueryPointer( display, DefaultRootWindow( display ), &wDummy1, &wDummy2, &iDummy3, &iDummy4, &iDummy5, &iDummy6, &iMask );
		setChecked( ( iMask & iKeyMask ) != 0 );
	}

	XFreeModifiermap( map );
};

void QVirtualButton::mousePressEvent( QMouseEvent *mEvent ) {

	/* Set pressed styleSheet */
	QColor hColor = mColor;
	hColor.setAlpha( 0.3 );
	setStyleSheet( QString(
			"color: %1; border: 0.5px solid %1; border-radius: 2px; background-color: rgba(%2, %3, %4, 100);"
		).arg( mColor.name() ).arg( mColor.red() ).arg( mColor.green() ).arg( mColor.blue() )
	);

	mEvent->accept();
};

void QVirtualButton::mouseReleaseEvent( QMouseEvent *mEvent ) {

	/* Reset to original StyleSheet */
	setStyleSheet( QString( "color: %1; border: 0.5px solid %1; border-radius: 2px;" ).arg( mColor.name() ) );

	if ( rect().contains( mEvent->pos() ) ) {
		emit clicked();

		/* Disable sendKey( ... ) signal for modifiers and quit */
		if ( modifiers.contains( mKeyCode ) ) {
			if ( isChecked() )
				setChecked( false );

			else
				setChecked( true );
		}

		/* Doing nothing for the quit key */
		else if ( mKeyCode == 999 ) {
			/* Nothing */
		}

		/* CapsLock */
		else if ( mKeyCode == 66 ) {
			if ( isChecked() )
				setChecked( false );

			else
				setChecked( true );

			/* We do need to send this key */
			emit sendKey( mKeyCode );
		}

		else
			emit sendKey( mKeyCode );
	}

	mEvent->accept();
};

void QVirtualButton::initStaticMembers() {

	if ( mInitDone )
		return;

	/* ToDo: we should replace this by a theme file read by QSettings */

	SizeKeyMap[ 9 ] = QSizeF( 16.0 / 17.0, 0.7 );		// Escape
	SizeKeyMap[ 67 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F1
	SizeKeyMap[ 68 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F2
	SizeKeyMap[ 69 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F3
	SizeKeyMap[ 70 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F4
	SizeKeyMap[ 71 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F5
	SizeKeyMap[ 72 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F6
	SizeKeyMap[ 73 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F7
	SizeKeyMap[ 74 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F8
	SizeKeyMap[ 75 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F9
	SizeKeyMap[ 76 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F10
	SizeKeyMap[ 95 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F11
	SizeKeyMap[ 96 ] = QSizeF( 16.0 / 17.0, 0.7 );		// F12
	SizeKeyMap[ 107 ] = QSizeF( 16.0 / 17.0, 0.7 );		// PrtScr
	SizeKeyMap[ 118 ] = QSizeF( 16.0 / 17.0, 0.7 );		// Insert
	SizeKeyMap[ 119 ] = QSizeF( 16.0 / 17.0, 0.7 );		// Delete
	SizeKeyMap[ 999 ] = QSizeF( 16.0 / 17.0, 0.7 );		// Quit Kbd
	SizeKeyMap[ 22 ] = QSizeF( 1.5, 1.0 );				// BackSpace
	SizeKeyMap[ 23 ] = QSizeF( 1.5, 1.0 );				// Tab
	SizeKeyMap[ 888 ] = QSizeF( 0.9, 1.0 );				// Blank
	SizeKeyMap[ 66 ] = QSizeF( 1.5, 1.0 );				// CapsLock
	SizeKeyMap[ 104 ] = QSizeF( 1.5, 1.0 );				// Enter
	SizeKeyMap[ 50 ] = QSizeF( 2.0, 1.0 );				// L Shift
	SizeKeyMap[ 62 ] = QSizeF( 2.0, 1.0 );				// R Shift
	SizeKeyMap[ 65 ] = QSizeF( 5.0, 1.0 );				// Space

	ColorKeyMap[ 9 ] = QColor( 202, 45, 223 );			// Escape
	ColorKeyMap[ 67 ] = QColor( 0, 128, 128 );			// F1
	ColorKeyMap[ 68 ] = QColor( 0, 128, 128 );			// F2
	ColorKeyMap[ 69 ] = QColor( 0, 128, 128 );			// F3
	ColorKeyMap[ 70 ] = QColor( 0, 128, 128 );			// F4
	ColorKeyMap[ 71 ] = QColor( 0, 128, 128 );			// F5
	ColorKeyMap[ 72 ] = QColor( 0, 128, 128 );			// F6
	ColorKeyMap[ 73 ] = QColor( 0, 128, 128 );			// F7
	ColorKeyMap[ 74 ] = QColor( 0, 128, 128 );			// F8
	ColorKeyMap[ 75 ] = QColor( 0, 128, 128 );			// F9
	ColorKeyMap[ 76 ] = QColor( 0, 128, 128 );			// F10
	ColorKeyMap[ 95 ] = QColor( 0, 128, 128 );			// F11
	ColorKeyMap[ 96 ] = QColor( 0, 128, 128 );			// F12
	ColorKeyMap[ 107 ] = QColor( 202, 45, 223 );		// PrtScr
	ColorKeyMap[ 118 ] = QColor( 202, 45, 223 );		// Insert
	ColorKeyMap[ 119 ] = QColor( 202, 45, 223 );		// Delete
	ColorKeyMap[ 999 ] = QColor( 255, 0, 0 );			// Quit Kbd
	ColorKeyMap[ 22 ] = QColor( 0, 191, 255 );			// BackSpace
	ColorKeyMap[ 23 ] = QColor( 133, 30, 22 );			// Tab
	ColorKeyMap[ 66 ] = QColor( 255, 140, 0 );			// CapsLock
	ColorKeyMap[ 104 ] = QColor( 0, 180, 0 );			// Enter
	ColorKeyMap[ 50 ] = QColor( 0, 0, 210 );			// L Shift
	ColorKeyMap[ 62 ] = QColor( 0, 0, 210 );			// R Shift
	ColorKeyMap[ 37 ] = QColor( 0, 0, 210 );			// L Ctrl
	ColorKeyMap[ 133 ] = QColor( 0, 0, 210 );			// L Super
	ColorKeyMap[ 64 ] = QColor( 0, 0, 210 );			// L Alt
	ColorKeyMap[ 65 ] = QColor( 176, 108, 30 );			// Space
	ColorKeyMap[ 108 ] = QColor( 0, 0, 210 );			// R Alt
	ColorKeyMap[ 135 ] = QColor( 0, 0, 210 );			// Menu
	ColorKeyMap[ 109 ] = QColor( 0, 0, 210 );			// R Ctrl

	mInitDone = true;
};
