/*
	*
	* This file is a part of CuboCore Project
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

// Local Headers
#include "CCVirtualKeyboard.hpp"

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QSettings>
#include <QSystemTrayIcon>

const int DEFAULT_KEY_WIDTH = 36;
const int DEFAULT_KEY_HEIGHT = 36;

CCKeyboardButton::CCKeyboardButton( unsigned int code, QString name, QWidget *parent ) : QPushButton( parent ) {

	value = code;

	setText( name );
	connect( this, SIGNAL( clicked() ), this, SLOT( translateKeyStroke() ) );

	setMinimumSize( DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT );
};

void CCKeyboardButton::translateKeyStroke() {

	emit sendKey( int( value ) );
};

CCVirtualKeyboard::CCVirtualKeyboard( QWidget *parent ) : QWidget( parent ) {

	createKeyboard();
	setWidgetProperties();
};

void CCVirtualKeyboard::createKeyboard() {

	xkbd = new X11Keyboard( this );

	QSettings keymap( ":/resources/en_US.keymap", QSettings::NativeFormat );

	QVBoxLayout *keyboardLyt = new QVBoxLayout();
	keyboardLyt->setSpacing( 0 );

	Q_FOREACH( QString row, keymap.value( "Rows" ).toStringList() ) {

		keymap.beginGroup( row );

		QHBoxLayout *lyt = new QHBoxLayout();
		lyt->setContentsMargins( QMargins() );
		lyt->setSpacing( 0 );

		Q_FOREACH( QString key, keymap.value( "Order" ).toStringList() ) {

			QString keyName = keymap.value( key ).toString();
			unsigned int keyCode = key.toInt();

			CCKeyboardButton *btn = new CCKeyboardButton( keyCode, keyName, this );
			connect( btn, SIGNAL( sendKey( unsigned int ) ), xkbd, SLOT( processKeyPress( unsigned int ) ) );

			/* Function Keys */
			if ( keyName.startsWith( "F" ) and not ( keyName == "F" ) ) {
				btn->setMinimumWidth( DEFAULT_KEY_WIDTH - 2 );
				btn->setFixedHeight( ( int )( DEFAULT_KEY_HEIGHT * 0.7 ) );
			}

			/* Escape, Insert, Delete, Print Screen */
			else if ( keyName.startsWith( "Esc" ) or keyName.startsWith( "Ins" ) or keyName.startsWith( "Del" ) or ( keyCode == 107 ) ) {
				btn->setMinimumWidth( DEFAULT_KEY_WIDTH - 2 );
				btn->setFixedHeight( ( int )( DEFAULT_KEY_HEIGHT * 0.7 ) );
			}

			/* Quit keyboard */
			else if ( keyCode == 999 ) {
				btn->setStyleSheet( "color: red;" );
				btn->setMinimumWidth( DEFAULT_KEY_WIDTH - 2 );
				btn->setFixedHeight( ( int )( DEFAULT_KEY_HEIGHT * 0.7 ) );

				disconnect( btn, SIGNAL( sendKey( unsigned int ) ), xkbd, SLOT( processKeyPress( unsigned int ) ) );
				connect( btn, SIGNAL( clicked() ), qApp, SLOT( quit() ) );
			}

			/* Backspace */
			else if ( keyCode == 22 ) {
				btn->setMinimumWidth( DEFAULT_KEY_WIDTH * 2 );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );
			}

			/* Tab */
			else if ( keyCode == 23 ) {
				btn->setMinimumWidth( ( int )DEFAULT_KEY_WIDTH * 1.5 );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );
			}

			/* Blank tile */
			else if ( keyCode == 888 ) {
				btn->setStyleSheet( "border: none;" );
				btn->setFixedWidth( ( int )DEFAULT_KEY_WIDTH * 0.9 );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );

				disconnect( btn, SIGNAL( sendKey( unsigned int ) ), xkbd, SLOT( processKeyPress( unsigned int ) ) );
				btn->setDisabled( true );
			}

			/* Caps */
				else if ( keyName.startsWith( "Caps" ) or ( keyCode == 104 ) ) {
				btn->setMinimumWidth( ( int )DEFAULT_KEY_WIDTH * 1.5 );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );
			}

			/* Shift */
			else if ( keyName.startsWith( "Shift" ) ) {
				btn->setMinimumWidth( ( int )DEFAULT_KEY_WIDTH * 2 );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );
			}

			/* SpaceBar */
			else if ( keyName.startsWith( "Space" ) ) {
				btn->setMinimumWidth( DEFAULT_KEY_WIDTH * 5 );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );
			}

			/* All other keys */
			else {
				btn->setFixedWidth( DEFAULT_KEY_WIDTH );
				btn->setFixedHeight( DEFAULT_KEY_HEIGHT );
			}

			lyt->addWidget( btn );
		}

		keyboardLyt->addLayout( lyt );

		keymap.endGroup();
	}

	setLayout( keyboardLyt );
};

void CCVirtualKeyboard::setWidgetProperties() {

	setWindowTitle( "CuboCore Virtual Keyboard" );
	setWindowIcon( QIcon( ":/resources/tray.png" ) );

	setAttribute( Qt::WA_ShowWithoutActivating );
	setWindowFlags( Qt::ToolTip | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );

	QRect scrnSize = qApp->desktop()->availableGeometry();
	int hpos = scrnSize.width() - 612;
	int vpos = scrnSize.height() - 220;

	move( hpos, vpos );
	setFixedSize( 612, 220 );
};

int main( int argc, char **argv ) {

	QApplication app( argc, argv );

	CCVirtualKeyboard Gui;
	Gui.show();

	QSystemTrayIcon *tray = new QSystemTrayIcon();
	tray->setIcon( QIcon( ":/resources/tray.png" ) );
	tray->show();

	qDebug() << Gui.size();

	return app.exec();
}
