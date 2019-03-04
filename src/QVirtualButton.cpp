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

#include <QDebug>
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QX11Info>

#define XK_Caps_Lock 0xffe5

Q_DECLARE_METATYPE( QList<int> )

QMap<unsigned int, QSizeF> QVirtualButton::SizeKeyMap = QMap<unsigned int, QSizeF>();
QMap<unsigned int, QColor> QVirtualButton::ColorKeyMap = QMap<unsigned int, QColor>();

QList<unsigned int> QVirtualButton::modifiers = QList<unsigned int>();
QList<unsigned int> QVirtualButton::doubleKeys = QList<unsigned int>();

bool QVirtualButton::mInitDone = false;

QVirtualButton::QVirtualButton( unsigned int code, QStringList names, QWidget *parent ) : QLabel( parent ) {

	/* Flags and state init */
	mModifier = false;
	mChecked = false;
	mCheckable = false;
	mPressed = false;
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
	setText( names );

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

	longPressTimer = new QTimer( this );
	longPressTimer->setSingleShot( true );
	longPressTimer->setInterval( 300 );

	if ( doubleKeys.contains( mKeyCode ) )
		connect( longPressTimer, SIGNAL( timeout() ), this, SLOT( autoReleaseKey() ) );

	setFont( QFont( "sans", 7 ) );
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

	else {
		setStyleSheet( QString( "color: %1; border: 0.5px solid %1; border-radius: 2px;" ).arg( mColor.name() ) );
	}

	setFont( QFont( "sans", 7 ) );
};

void QVirtualButton::setText( QStringList names ) {

	mName = names[ 0 ];
	QLabel::setText( mName );

	if ( names.length() > 1 ) {
		setAlignment( Qt::AlignLeft | Qt::AlignBottom );
		mNameAlt = names[ 1 ];
	}

	else {
		setAlignment( Qt::AlignCenter );
	}
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

	repaint();
};

void QVirtualButton::autoReleaseKey() {

	if ( rect().contains( mapFromGlobal( QCursor::pos() ) ) ) {
		emit clicked();
		emit longPressed();
	}

	mPressed = false;

	/* Stop long-press timer */
	longPressTimer->stop();

	/* Reset to original StyleSheet */
	setStyleSheet( QString( "color: %1; border: 0.5px solid %1; border-radius: 2px;" ).arg( mColor.name() ) );

	setFont( QFont( "sans", 7 ) );

	repaint();
};

void QVirtualButton::mousePressEvent( QMouseEvent *mEvent ) {

	/* Set pressed styleSheet */
	QColor hColor = mColor;
	hColor.setAlpha( 0.3 );
	setStyleSheet( QString(
			"color: %1; border: 0.5px solid %1; border-radius: 2px; background-color: rgba(%2, %3, %4, 100);"
		).arg( mColor.name() ).arg( mColor.red() ).arg( mColor.green() ).arg( mColor.blue() )
	);

	mPressed = true;
	longPressTimer->start();

	mEvent->accept();
	setFont( QFont( "sans", 7 ) );

	repaint();
};

void QVirtualButton::mouseReleaseEvent( QMouseEvent *mEvent ) {

	/* To enable long-press keys */
	if ( not mPressed )
		return;

	/* Stop long-press timer */
	longPressTimer->stop();

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
	setFont( QFont( "sans", 7 ) );

	repaint();
};

void QVirtualButton::paintEvent( QPaintEvent *pEvent ) {

	QLabel::paintEvent( pEvent );

	QPainter *painter = new QPainter( this );

	painter->save();
	painter->setRenderHints( QPainter::Antialiasing );

	QRect rectAlt = QRect( QPoint( size().width() / 2, 0 ), size() / 2 );
	if ( mNameAlt.count() )
		painter->drawText( rectAlt, Qt::AlignCenter, mNameAlt );

	painter->restore();
	painter->end();
};

void QVirtualButton::initStaticMembers() {

	if ( mInitDone )
		return;

	modifiers << 50 << 62 << 37 << 133 << 64 << 108 << 135 << 109;

	doubleKeys << 10 << 11 << 12 << 13 << 14 << 15 << 16 << 17 << 18 << 19 << 20 << 21 << 24 << 25;
	doubleKeys << 26 << 27 << 28 << 29 << 30 << 31 << 32 << 33 << 34 << 35 << 38 << 39 << 40 << 41;
	doubleKeys << 42 << 43 << 44 << 45 << 46 << 47 << 48 << 49 << 51 << 52 << 53 << 54 << 55 << 56;
	doubleKeys << 57 << 58 << 59 << 60 << 61;

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
