#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "vkeyboard.h"

#include <QWidget>

class MainWidget : public QWidget {

	Q_OBJECT
	public:
		explicit MainWidget(QWidget *parent = 0);
		void setBaseSize(int w, int h);

	signals:

	public slots:
		void textSwitch(bool);
		void updateLayout(int index, QString layout_name);
		void updateGroupState(const ModifierGroupStateMap&);
		void updateFont(const QFont& widgetFont);

	protected:
		virtual void resizeEvent(QResizeEvent *ev);
		QSize bsize;

};
#endif // MAINWIDGET_H
