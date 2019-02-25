/*
	*
	* QVirtualKeyboard.hpp - On-Screen Keyboard for X11 based on Qt
	*
*/

#pragma once

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QSystemTrayIcon>
#include "QVirtualButton.hpp"

class QVKTrayIcon : public QSystemTrayIcon {
	Q_OBJECT

	public:
		QVKTrayIcon( QWidget *parent ) : QSystemTrayIcon( parent ) {

			setIcon( QIcon( ":/resources/tray.png" ) );
			show();

			connect( this, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( activationHandler( QSystemTrayIcon::ActivationReason ) ) );
		};

	private Q_SLOTS:
		void activationHandler( QSystemTrayIcon::ActivationReason reason ) {

			if ( reason == QSystemTrayIcon::Trigger )
				emit toggleShowHide();
		};

	Q_SIGNALS:
		void toggleShowHide();
};

class QVirtualKeyboard : public QWidget {
	Q_OBJECT

	public :
		QVirtualKeyboard( QWidget *parent = 0  );

	private :
		void createKeyboard();
		void setWidgetProperties();

		void sendKey( unsigned int );

		QList<QVirtualButton *> modifiers;

		QPoint dragPoint;
		QPoint gpress;

		bool dragged;
		bool moved;

		QVKTrayIcon *trayIcon;

	protected:
		void mouseMoveEvent( QMouseEvent * );
		void mousePressEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );

	private Q_SLOTS:
		void processKeyPress( unsigned int );
		void toggleShowHide();
};
