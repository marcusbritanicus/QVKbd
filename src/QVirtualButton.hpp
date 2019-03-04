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
#include <QTimer>

static const int DEFAULT_KEY_WIDTH = 32;
static const int DEFAULT_KEY_HEIGHT = 32;

class QVirtualButton : public QLabel {
	Q_OBJECT

	public:
		QVirtualButton( unsigned int code, QStringList names, QWidget *parent );

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

		/* Double text for keys */
		void setText( QStringList );

		/* Key width for widget width */
		static void idealKeyWidth( int );

	private:
		/* Key label */
		QString mName;
		QString mNameAlt;

		/* X11 KeyCode for a given Key */
		unsigned int mKeyCode;

		/* Modifier flag */
		bool mModifier;

		/* Checked state */
		bool mChecked;

		/* Checkable flag */
		bool mCheckable;

		/* Pressed flag */
		bool mPressed;

		/* Checkable flag */
		static bool mInitDone;

		/* Color of the button */
		QColor mColor;

		/* Timer for long press */
		QTimer *longPressTimer;

		/* Modifiers and Keys with double values */
		static QList<unsigned int> modifiers;
		static QList<unsigned int> doubleKeys;

		/* Init static numbers */
		static void initStaticMembers();

		/* This has the relative size ratios for various keys */
		/* In case we use a scalable keyboard, we can use this object*/
		/* QSize contains the multiplication ratio, w.r.t default sizes */
		static QMap<unsigned int, QSizeF> SizeKeyMap;

		/* Color Key Maps: Color for each key */
		/* If there is no color defined, then it will be taken as black */
		/* Modifiers: Dark Blue, Space: Dark Green, Del, Bksp: Dark Red, Quit: Red */
		/* Tab: Sky Blue, Caps Lock: Orange, Functions: Dark Blue */
		static QMap<unsigned int, QColor> ColorKeyMap;

	public Q_SLOTS:
		/* Auto-release key on long press */
		void autoReleaseKey();

	private Q_SLOTS:
		/* Auto-set CapsLock state */
		void setCapsLockState();

	protected:
		/* mouseReleaseEvent to simulate click */
		void mousePressEvent( QMouseEvent * );

		/* mouseReleaseEvent to simulate click */
		void mouseReleaseEvent( QMouseEvent * );

		/* paintEvent to have double labels */
		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		/* Signal when the key is pressed */
		void sendKey( unsigned int );

		/* clicked() signal */
		void clicked();

		/* enableVirtualShift() signal */
		void longPressed();
};
