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

#include "x11keyboard.h"

#include <QX11Info>
#include <QDesktopWidget>
#include <QDebug>

#include <X11/extensions/XTest.h>
#include <X11/Xlocale.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>

#include <X11/XKBlib.h>

#include <iostream>
using namespace std;

X11Keyboard::X11Keyboard( QObject *parent ): QObject( parent ) {

	groupTimer = new QTimer( parent );
	groupTimer->setInterval( 250 );

	groupState.insert( "capslock", this->queryModKeyState( XK_Caps_Lock ) );
	groupState.insert( "numlock", this->queryModKeyState( XK_Num_Lock ) );

	connect( groupTimer, SIGNAL( timeout() ), this, SLOT( queryModState() ) );
}

X11Keyboard::~X11Keyboard() {
}

void X11Keyboard::start() {

	emit groupStateChanged( groupState );
	groupTimer->start();
}

void X11Keyboard::constructLayouts() {
}

void X11Keyboard::processKeyPress( unsigned int keyCode ) {

	groupTimer->stop();
	sendKey( keyCode );
	emit keyProcessComplete( keyCode );
	groupTimer->start();
}

void X11Keyboard::sendKey( unsigned int keycode ) {

	Window currentFocus;
	int revertTo;

	Display *display = QX11Info::display();
	XGetInputFocus( display, &currentFocus, &revertTo );

	// QListIterator<VButton *> itr( modKeys );
	// while ( itr.hasNext() ) {
		// VButton *mod = itr.next();
		// if ( mod->isChecked() ) {
			// XTestFakeKeyEvent( display, mod->getKeyCode(), true, 2 );
		// }
	// }

	XTestFakeKeyEvent( display, keycode, true, 2 );
	XTestFakeKeyEvent( display, keycode, false, 2 );

	// itr.toFront();
	// while ( itr.hasNext() ) {
		// VButton *mod = itr.next();
		// if ( mod->isChecked() )
			// XTestFakeKeyEvent( display, mod->getKeyCode(), false, 2 );
	// }

	XFlush( display );
}

bool X11Keyboard::queryModKeyState( KeySym iKey ) {

	int          iKeyMask = 0;
	Window       wDummy1, wDummy2;
	int          iDummy3, iDummy4, iDummy5, iDummy6;
	unsigned int iMask;

	Display* display = QX11Info::display();

	XModifierKeymap* map = XGetModifierMapping( display );
	KeyCode keyCode = XKeysymToKeycode( display, iKey );

	if ( keyCode == NoSymbol )
		return false;

	for ( int i = 0; i < 8; ++i ) {
		if ( map->modifiermap[map->max_keypermod * i] == keyCode )
			iKeyMask = 1 << i;
	}

	XQueryPointer( display, DefaultRootWindow( display ), &wDummy1, &wDummy2, &iDummy3, &iDummy4, &iDummy5, &iDummy6, &iMask );
	XFreeModifiermap( map );

	return ( ( iMask & iKeyMask ) != 0 );
}

void X11Keyboard::queryModState() {

	bool curr_caps_state = this->queryModKeyState( XK_Caps_Lock );
	bool curr_num_state = this->queryModKeyState( XK_Num_Lock );

	bool caps_state = groupState.value( "capslock" );
	bool num_state = groupState.value( "numlock" );

	groupState.insert( "capslock", curr_caps_state );
	groupState.insert( "numlock", curr_num_state );

	if ( curr_caps_state != caps_state || curr_num_state != num_state )
		emit groupStateChanged( groupState );
}

void X11Keyboard::textForKeyCode( unsigned int keyCode, ButtonText& text ) {

	if ( keyCode==0 ) {
		text.clear();
		return;
	}

	KeyCode button_code = keyCode;

	int keysyms_per_keycode = 0;

	KeySym *keysym = XGetKeyboardMapping ( QX11Info::display(), button_code, 1, &keysyms_per_keycode );

	int index_normal = layout_index * 2;
	int index_shift = index_normal + 1;

	KeySym normal = keysym[index_normal];
	KeySym shift = keysym[index_shift];


	long int ret = kconvert.convert( normal );
	long int shiftRet = kconvert.convert( shift );

	QChar normalText = QChar( ( uint )ret );
	QChar shiftText = QChar( ( uint )shiftRet );

	//cout << "Normal Text " << normalText.toAscii() << " Shift Text: " << shiftText.toAscii() << std::endl;

	text.clear();
	text.append( normalText );
	text.append( shiftText );

	XFree ( ( char * ) keysym );
}
