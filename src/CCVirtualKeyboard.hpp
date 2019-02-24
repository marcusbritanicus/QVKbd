/*
	*
	* CCVirtualKeyboard.hpp - On-Screen Keyboard for CuboCore Project.
	*
*/

#pragma once

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QDebug>

#include "x11keyboard.h"

class CCKeyboardButton : public QPushButton {
	Q_OBJECT

	public:
		CCKeyboardButton( unsigned int code, QString name, QWidget *parent ) : QPushButton( parent ) {

			value = code;

			setText( name );
			connect( this, SIGNAL( clicked() ), this, SLOT( translateKeyStroke() ) );

			setMinimumSize( QSize( 36, 36 ) );
		}

	private Q_SLOTS:
		void translateKeyStroke() {

			emit sendKey( int( value ) );
		};

	private:
		unsigned int value;

	Q_SIGNALS:
		void sendKey( unsigned int );
};

class CCVirtualKeyboard : public QWidget {
	Q_OBJECT

	public :
		CCVirtualKeyboard( QWidget *parent = 0  );

	private :
		void createKeyboard();
		void setWidgetProperties();

		X11Keyboard *xkbd;
};
