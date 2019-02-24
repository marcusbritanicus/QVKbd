/*
	*
	* This file is modified by Marcus Britanicus <marcusbritanicus@gmail.com>
	* for use as a part of CuboCore Project.
	*
	* This file was originally written by Todor Gyumyushev <yodor1@gmail.com>
	* as a part of Kvkbd project (https://github.com/KDE/kvkbd)
	*
	* This program is free software: you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation, either version 3 of the License, or
	* (at your option) any later version.
	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	* GNU General Public License for more details.
	*
	* You should have received a copy of the GNU General Public License
	* along with this program. If not, see <http://www.gnu.org/licenses/>.
	*
*/

#pragma once

#include "keysymconvert.h"
// #include "fixx11h.h"

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QChar>
#include <QMap>

// CapsLock and NumLock states
typedef QMap<QString, bool> ModifierGroupStateMap;
typedef QMapIterator<QString, bool> ModifierGroupStateMapIterator;

// Normal text, Shift text
typedef QList<QChar> ButtonText;

class X11Keyboard : public QObject {
    Q_OBJECT

	public:
		X11Keyboard( QObject *parent = 0 );
		virtual ~X11Keyboard();

		void textForKeyCode( unsigned int keyCode, ButtonText& text );

	public slots:
		virtual void processKeyPress(unsigned int);
		virtual void queryModState();
		virtual void constructLayouts();
		virtual void start();

	protected:
		void sendKey( unsigned int keycode );

		QStringList layouts;
		int layout_index;

		KeySymConvert kconvert;

		bool queryModKeyState( KeySym keyCode );
		ModifierGroupStateMap groupState;
		QTimer *groupTimer;

	signals:
		void keyProcessComplete( unsigned int );
		void groupStateChanged( const ModifierGroupStateMap& modifier_state );
};
