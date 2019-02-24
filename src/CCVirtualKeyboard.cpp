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

CCVirtualKeyboard::CCVirtualKeyboard( QWidget *parent ) : QWidget( parent ) {

	createKeyboard();
	setWidgetProperties();
};

void CCVirtualKeyboard::createKeyboard() {

	xkbd = new X11Keyboard( this );

	QSettings keymap( ":/resources/en_US.keymap", QSettings::NativeFormat );

	QVBoxLayout *keyboardLyt = new QVBoxLayout();

	Q_FOREACH( QString row, keymap.value( "Rows" ).toStringList() ) {

		keymap.beginGroup( row );

		QHBoxLayout *lyt = new QHBoxLayout();
		lyt->addStretch();

		qDebug() << keymap.value( "Order" ).toStringList();

		Q_FOREACH( QString key, keymap.value( "Order" ).toStringList() ) {

			CCKeyboardButton *btn = new KeyboardButton( key.toInt(), keymap.value( key ).toString(), this );
			connect( btn, SIGNAL( sendKey( unsigned int ) ), xkbd, SLOT( processKeyPress( unsigned int ) ) );
			lyt->addWidget( btn );
		}

		lyt->addStretch();
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

	QDesktopWidget dw;
	QRect scrnSize = dw.screenGeometry();
	int w = scrnSize.width();
	int h = scrnSize.height();
	int hpos = (int)((w - 700) / 2);
	int vpos = (int)((h - 400) / 2);
	setGeometry(hpos, vpos, 700, 400);
	setFixedSize( 600, 300 );
};

int main( int argc, char **argv ) {

	QApplication app( argc, argv );

	CCVirtualKeyboard Gui;
	Gui.show();

	QSystemTrayIcon *tray = new QSystemTrayIcon();
	tray->setIcon( QIcon( ":/resources/tray.png" ) );
	tray->show();

	return app.exec();
}
