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
#include <QMouseEvent>
#include <QLabel>
#include <QMap>

class QVirtualButton : public QLabel {
	Q_OBJECT

	public:
		QVirtualButton( unsigned int code, QString name, QWidget *parent );

		/* X11 KeyCode for a given key */
		unsigned int keyCode();

		/* Check if the key is a modifier */
		bool isModifier();

		/* Set/Unset the key as a modifier */
		void setModifier( bool );

		/* Set/Unset the key as checkable */
		void setCheckable( bool );

		/* Check if the key is pressed */
		bool isChecked();

		/* Set/Unset the key pressed state */
		void setChecked( bool );

	private:
		/* Key label */
		QString mName;

		/* X11 KeyCode for a given Key */
		unsigned int mKeyCode;

		/* Modifier flag */
		bool mModifier;

		/* Checked state */
		bool mChecked;

		/* Checkable flag */
		bool mCheckable;

		/* Checkable flag */
		static bool mInitDone;

		/* Color of the button */
		QColor mColor;

		/* Init static numbers */
		static void initStaticMembers();

		static QList<unsigned int> modifiers;

		/* This has the relative size ratios for various keys */
		/* In case we use a scalable keyboard, we can use this object*/
		/* QSize contains the multiplication ratio, w.r.t default sizes */
		static QMap<unsigned int, QSizeF> SizeKeyMap;

		/* Color Key Maps: Color for each key */
		/* If there is no color defined, then it will be taken as black */
		/* Modifiers: Dark Blue, Space: Dark Green, Del, Bksp: Dark Red, Quit: Red */
		/* Tab: Sky Blue, Caps Lock: Orange, Functions: Dark Blue */
		static QMap<unsigned int, QColor> ColorKeyMap;

	private Q_SLOTS:
		/* s */
		void setCapsLockState();

	protected:
		/* mouseReleaseEvent to simulate click */
		void mousePressEvent( QMouseEvent * );

		/* mouseReleaseEvent to simulate click */
		void mouseReleaseEvent( QMouseEvent * );

	Q_SIGNALS:
		/* Signal when the key is pressed */
		void sendKey( unsigned int );

		/* clicked() signal */
		void clicked();
};
