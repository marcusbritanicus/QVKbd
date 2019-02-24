/*
 * This file is part of the Kvkbd project.
 * Copyright ( C ) 2007-2014 Todor Gyumyushev <yodor1@gmail.com>
 * Copyright ( C ) 2008 Guillaume Martres <smarter@ubuntu.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * ( at your option ) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "kvkbdapp.h"

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <fixx11h.h>

QList<VButton*> modKeys;
#include <iostream>

using namespace std;

#define DEFAULT_WIDTH 	640
#define DEFAULT_HEIGHT 	210

#include "x11keyboard.h"

KvkbdApp::KvkbdApp( int argc, char *argv[] ) : QApplication( argc, argv ) {

	signalMapper = new QSignalMapper( this );
	connect( signalMapper, SIGNAL( mapped( const QString & ) ), this, SLOT( buttonAction( const QString & ) ) );

	widget = new ResizableDragWidget( 0 );
	widget->setContentsMargins( 10, 10, 10, 10 );
	widget->setProperty( "name", "main" );

	widget->setAttribute( Qt::WA_ShowWithoutActivating );
	widget->setAttribute( Qt::WA_DeleteOnClose, false );

	widget->setWindowFlags(  Qt::ToolTip | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint  );

	tray = new KbdTray( widget );
	connect( tray, SIGNAL( requestVisibility() ), widget, SLOT( toggleVisibility() ) );

	layout = new QGridLayout( widget );
	layout->setContentsMargins( 0, 0, 0, 0 );
	widget->setLayout( layout );

	xkbd = new X11Keyboard( this );

	themeLoader = new ThemeLoader( widget );
	connect( themeLoader, SIGNAL( partLoaded( MainWidget*, int, int ) ), this, SLOT( partLoaded( MainWidget*, int, int ) ) );
	connect( themeLoader, SIGNAL( buttonLoaded( VButton* ) ), this, SLOT( buttonLoaded( VButton* ) ) );

	QMenu *cmenu = tray->contextMenu();

	QLabel *titleLbl = new QLabel( "<b>Kvkbd</b>" );
	titleLbl->setAlignment( Qt::AlignCenter );
	titleLbl->setFixedHeight( 24 );

	QWidgetAction *titleAction = new QWidgetAction( cmenu );
	titleAction->setDefaultWidget( titleLbl );
    cmenu->addAction( titleAction );

    cmenu->addSeparator();

	QAction *chooseFontAction = new QAction( QIcon::fromTheme( "preferences-desktop-font" ), "Choose Font...", this );
	connect( chooseFontAction, SIGNAL( triggered( bool ) ), this, SLOT( chooseFont() )  );
	cmenu->addAction( chooseFontAction );

	QAction *autoResizeAction = new QAction( "Auto Resize Font", this );
	bool autoResizeEnabled = true;
	autoResizeAction->setCheckable( true );
	autoResizeAction->setChecked( autoResizeEnabled );
	widget->setProperty( "autoresfont", autoResizeEnabled );
	cmenu->addAction( autoResizeAction );
	connect( autoResizeAction,SIGNAL( triggered( bool ) ), this, SLOT( autoResizeFont( bool ) ) );

    cmenu->addSeparator();

	bool blur = false;
	QAction *blurBackgroundAction = new QAction( "Blur Background", this );
	blurBackgroundAction->setCheckable( true );
	blurBackgroundAction->setChecked( blur );
	cmenu->addAction( blurBackgroundAction );

	widget->blurBackground( blur );
	connect( blurBackgroundAction,SIGNAL( triggered( bool ) ), widget, SLOT( blurBackground( bool ) ) );

	widget->blurBackground( blur );

	bool isLocked = false;
	QAction *lockOnScreenAction = new QAction( "Lock on Screen", this );
	lockOnScreenAction->setCheckable( true );
	lockOnScreenAction->setChecked( isLocked );
	cmenu->addAction( lockOnScreenAction );
	connect( lockOnScreenAction,SIGNAL( triggered( bool ) ), widget, SLOT( setLocked( bool ) ) );

	bool stickyModKeys = false; //cfg.readEntry( "stickyModKeys", QVariant( false ) ).toBool();
	QAction *stickyModKeysAction = new QAction( "Sticky Modifier Keys", this );
	stickyModKeysAction->setCheckable( true );
	stickyModKeysAction->setChecked( stickyModKeys );
	cmenu->addAction( stickyModKeysAction );
	connect( stickyModKeysAction,SIGNAL( triggered( bool ) ), this, SLOT( setStickyModKeys( bool ) ) );
	widget->setProperty( "stickyModKeys", stickyModKeys );

	QString colorsFilename = ":/resources/standart.css";
	themeLoader->findColorStyles( new QMenu(), colorsFilename );

	cmenu->addSeparator();

	QAction *quitAction = new QAction( QIcon::fromTheme( "application-quit" ), "&Quit Kvkbd", this );
	connect( quitAction, SIGNAL( triggered() ), qApp, SLOT( quit() ) );
	cmenu->addAction( quitAction );

	QString themeName = "standart";
	themeLoader->loadTheme( themeName );
	widget->setProperty( "layout", themeName );

	QSize defaultSize( DEFAULT_WIDTH,DEFAULT_HEIGHT );
	QDesktopWidget *pDesktop = QApplication::desktop();
	QRect screenGeometry = pDesktop->availableGeometry( pDesktop->underMouse() );
	qDebug() << "ScreenGeometry: " << screenGeometry;

	QPoint bottomRight = screenGeometry.bottomRight()-QPoint( defaultSize.width(), defaultSize.height() );
	QRect widgetGeometry( bottomRight, defaultSize );
	qDebug() << "widgetGeometry: " << widgetGeometry;

	QRect c_geometry = widgetGeometry;
	if ( !screenGeometry.contains( c_geometry, true ) ) {
		c_geometry = widgetGeometry;
	}
	widget->setGeometry( c_geometry );

	widget->show();

	bool extensionVisible = false;
	if ( !extensionVisible )
		toggleExtension();

	setQuitOnLastWindowClosed( false );
	connect( this, SIGNAL( aboutToQuit() ), this, SLOT( storeConfig() ) );
	emit fontUpdated( widget->font() );

	xkbd->start();
	bool vis = true; //cfg.readEntry( "visible", QVariant( true ) ).toBool();

	if ( !vis  ) {
		widget->showMinimized();
	}
	widget->setWindowTitle( "Kvkbd" );
	tray->show();
}

KvkbdApp::~KvkbdApp()
{
}

void KvkbdApp::storeConfig()
{
    // KConfigGroup cfg = KGlobal::config()->group( "" );
    // cfg.writeEntry( "visible", widget->isVisible() );
    // cfg.writeEntry( "geometry", widget->geometry() );
    // cfg.writeEntry( "locked", widget->isLocked() );
    // cfg.writeEntry( "stickyModKeys", widget->property( "stickyModKeys" ) );
    // cfg.writeEntry( "layout", widget->property( "layout" ) );
    // cfg.writeEntry( "colors", widget->property( "colors" ) );
    // cfg.writeEntry( "font", widget->font() );
    // cfg.writeEntry( "autoresfont", widget->property( "autoresfont" ).toBool() );
    // cfg.writeEntry( "blurBackground", widget->property( "blurBackground" ).toBool() );

    // MainWidget *prt = parts.value( "extension" );
    // if ( prt ) {
        // cfg.writeEntry( "extentVisible", prt->isVisible() );
    // }

    // cfg.sync();
}

void KvkbdApp::autoResizeFont( bool mode )
{
    widget->setProperty( "autoresfont", QVariant( mode ) );
    emit fontUpdated( widget->font() );
}

void KvkbdApp::setStickyModKeys( bool mode )
{
    widget->setProperty( "stickyModKeys", QVariant( mode ) );
}

void KvkbdApp::chooseFont() {

    QFont newFont = QFontDialog::getFont( 0, widget->font() );
	widget->setFont( newFont );
	emit fontUpdated( newFont );
};

void KvkbdApp::buttonLoaded( VButton *btn ) {

    if ( btn->property( "modifier" ).toBool() == true ) {
        modKeys.append( btn );
    }

    else {
        QObject::connect( btn, SIGNAL( keyClick( unsigned int ) ), xkbd, SLOT( processKeyPress( unsigned int ) )  );
    }

    QString bAction = btn->property( "action" ).toString();

    if ( bAction.length()>0 ) {
        connect( btn, SIGNAL( clicked() ), signalMapper, SLOT( map() ) );
        signalMapper->setMapping( btn, bAction );
        actionButtons.insertMulti( bAction, btn );
    }

    QString tooltip = btn->property( "tooltip" ).toString();
    if ( tooltip.length() > 0 )
        btn->setToolTip( tooltip );
}

void KvkbdApp::partLoaded( MainWidget *vPart, int total_rows, int total_cols )
{
    //cout << "Col Strech: " << total_cols << endl;
    // cout << "Row Strech: " << total_rows << endl;
    QString partName = vPart->property( "part" ).toString();
    int row_pos = 0;
    int col_pos = 0;
    if ( layoutPosition.count()>0 ) {
        QString partName = layoutPosition.keys().at( 0 );
        QRect lp = layoutPosition.value( partName );
        col_pos = lp.width();
    }
    layout->addWidget( vPart,row_pos,col_pos,total_rows,total_cols );
    //cout << "Insert to layout: " << qPrintable( partName ) << " RowStart: " << row_pos << " ColStart: " << col_pos << " RowSpan: " << total_rows << " ColSpan: " << total_cols << endl;
    parts.insert( partName, vPart );
    layoutPosition.insert( partName, QRect( col_pos,row_pos,total_cols,total_rows ) );
    QObject::connect( xkbd, SIGNAL( layoutUpdated( int,QString ) ), vPart, SLOT( updateLayout( int,QString ) ) );
    QObject::connect( xkbd, SIGNAL( groupStateChanged( const ModifierGroupStateMap& ) ), vPart, SLOT( updateGroupState( const ModifierGroupStateMap& ) ) );
    QObject::connect( xkbd, SIGNAL( keyProcessComplete( unsigned int ) ), this, SLOT( keyProcessComplete( unsigned int ) ) );
    QObject::connect( this, SIGNAL( textSwitch( bool ) ), vPart, SLOT( textSwitch( bool ) ) );
    QObject::connect( this, SIGNAL( fontUpdated( const QFont& ) ), vPart, SLOT( updateFont( const QFont& ) ) );
}

void KvkbdApp::keyProcessComplete( unsigned int )
{
    if ( widget->property( "stickyModKeys" ).toBool() ) return;
    QListIterator<VButton *> itr( modKeys );
    while ( itr.hasNext() ) {
        VButton *mod = itr.next();
        if ( mod->isChecked() ) {
            mod->click();
        }
    }
}

void KvkbdApp::buttonAction( const QString &action ) {

	if ( QString::compare( action , "toggleVisibility" )==0 )
		widget->toggleVisibility();

    else if ( QString::compare( action , "toggleExtension" ) == 0 )
        toggleExtension();

    else if ( QString::compare( action, "shiftText" ) == 0 ) {
        if ( actionButtons.contains( action ) ) {
            QList<VButton*> buttons = actionButtons.values( action );
            QListIterator<VButton *> itr( buttons );
            bool setShift = false;
            while ( itr.hasNext() ) {
                VButton *btn = itr.next();
                if ( btn->isCheckable() && btn->isChecked() ) setShift=true;
            }
            emit textSwitch( setShift );
        }
    }
}

void KvkbdApp::toggleExtension()
{
    MainWidget *prt = parts.value( "extension" );
    if ( prt->isVisible() ) {
        prt->hide();
        layout->removeWidget( prt );
    }
    else {
        QString partName = prt->property( "part" ).toString();
        QRect span = layoutPosition.value( partName );
        layout->addWidget( prt,span.y(),span.x(), span.height(), span.width() );
        prt->show();
    }
}
