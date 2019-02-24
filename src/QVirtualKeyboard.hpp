/*
	*
	* QVirtualKeyboard.hpp - On-Screen Keyboard for X11 based on Qt
	*
*/

#pragma once

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QDebug>

class QKeyboardButton : public QPushButton {
	Q_OBJECT

	public:
		QKeyboardButton( unsigned int code, QString name, QWidget *parent );

		unsigned int keyCode() {

			return value;
		};

	private Q_SLOTS:
		void translateKeyStroke();

	private:
		unsigned int value;

	Q_SIGNALS:
		void sendKey( unsigned int );
};

class QVirtualKeyboard : public QWidget {
	Q_OBJECT

	public :
		QVirtualKeyboard( QWidget *parent = 0  );

	private :
		void createKeyboard();
		void setWidgetProperties();

		void sendKey( unsigned int );

		QList<unsigned int> modifierCodes;
		QList<QKeyboardButton *> modifiers;

	private Q_SLOTS:
		void processKeyPress( unsigned int );
};
