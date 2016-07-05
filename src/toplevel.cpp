/*
 * Copyright 1998-1999 by Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 * Copyright 2002-2003 by Martin Willers <willers@xm-arts.de>
 * Copyright 2007-2009 by Stefan Böhmann <kde@hilefoks.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "toplevel.h"
#include "timeedit.h"
#include "settings.h"
#include "tea.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QBrush>
#include <QDebug>
#include <QDesktopWidget>
#include <QMenu>
#include <QString>
#include <QTimer>

#include <KAboutData>
#include <KActionCollection>
#include <KConfigGroup>
#include <KHelpMenu>
#include <KIconLoader>
#include <KNotification>
#include <KNotifyConfigWidget>
#include <KPassivePopup>
#include <KSharedConfig>


TopLevel::TopLevel(const KAboutData *aboutData, const QString &icon, QWidget *parent)
  : KStatusNotifierItem( parent ),
    m_popup( new KPassivePopup ),
    m_runningTeaTime( 0 ),
    m_nextNotificationTime( 0 )
{
    setIconByPixmap(QIcon::fromTheme(icon));
    setCategory(ApplicationStatus);
    setStatus(Active);
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup tealistGroup( config, "Tealist" );

    if( tealistGroup.exists() ) {
        QString key, time;
        for(unsigned index=0; tealistGroup.hasKey(time.sprintf("Tea%d Time", index)) && tealistGroup.hasKey(key.sprintf("Tea%d Name", index)); ++index) {
            if( int temp = ( tealistGroup.readEntry( time, QString() ) ).toUInt() ) {
                m_tealist.append( Tea( tealistGroup.readEntry( key, i18n( "Unknown Tea" ) ), temp ) );
            }
        }
    }

    // If the list of teas is empty insert a set of default teas.
    if( m_tealist.isEmpty() ) {
        m_tealist.append( Tea(i18n( "Black Tea" ), 180 ) );
        m_tealist.append( Tea(i18n( "Earl Grey" ), 300 ) );
        m_tealist.append( Tea(i18n( "Fruit Tea" ), 480 ) );
        m_tealist.append( Tea(i18n( "Green Tea" ), 120 ) );
    }


    m_teaActionGroup=new QActionGroup( this );
    m_actionCollection = new KActionCollection(this);

    // build the context menu
    action = m_actionCollection->addAction( QStringLiteral( "stop" ));
    action->setIcon(QIcon::fromTheme( QStringLiteral( "edit-delete" ) ) );
    action->setText( i18n( "&Stop" ) );
    action->setEnabled( false );
    connect(action, &QAction::triggered, this, &TopLevel::cancelTea);

    action = m_actionCollection->addAction( QStringLiteral( "configure" ));
    action->setIcon(QIcon::fromTheme( QStringLiteral( "configure" ) ) );
    action->setText(i18n( "&Configure..." ) );
    connect(action, &QAction::triggered, this, &TopLevel::showSettingsDialog);

    action = KStandardAction::configureNotifications(this, &TopLevel::configureNotifications, m_actionCollection);
    action = KStandardAction::quit(qApp, &QCoreApplication::quit, m_actionCollection);
    action->setShortcut(0);

    action = m_actionCollection->addAction( QStringLiteral( "anonymous" ));
    action->setText(i18n( "&Anonymous..." ) );
    connect(action, &QAction::triggered, this, &TopLevel::showTimeEditDialog);

    m_helpMenu = new KHelpMenu( 0, *aboutData, false );

    setStandardActionsEnabled(false);
    loadTeaMenuItems();
    contextMenu()->addSeparator();
    contextMenu()->addAction( m_actionCollection->action(QStringLiteral("stop")) );
    contextMenu()->addAction( m_actionCollection->action(QStringLiteral("anonymous")) );
    contextMenu()->addSeparator();
    contextMenu()->addAction( m_actionCollection->action(QStringLiteral("configure")) );
    contextMenu()->addAction( m_actionCollection->action(QLatin1String(KStandardAction::name(KStandardAction::ConfigureNotifications))) );
    contextMenu()->addMenu( m_helpMenu->menu() );
    contextMenu()->addSeparator();
    contextMenu()->addAction( m_actionCollection->action(QLatin1String(KStandardAction::name(KStandardAction::Quit))) );

    m_timer = new QTimer( this );

    connect(m_timer, &QTimer::timeout, this, &TopLevel::teaTimeEvent);
    connect(contextMenu(), static_cast<void (QMenu::*)(QAction*)>(&QMenu::triggered), this, static_cast<void (TopLevel::*)(QAction*)>(&TopLevel::runTea));
    connect(this, &TopLevel::activateRequested, this, &TopLevel::showPopup);

    loadConfig();
    checkState();
}


TopLevel::~TopLevel()
{
    delete m_helpMenu;
    delete m_timer;
    delete m_popup;
    delete m_teaActionGroup;
}


void TopLevel::checkState() {
    const bool state = m_runningTeaTime != 0;

    m_teaActionGroup->setEnabled( !state );
    m_actionCollection->action(QStringLiteral("stop"))->setEnabled( state );
    m_actionCollection->action(QStringLiteral("anonymous"))->setEnabled( !state );

    if( !state ) {
        setTooltipText( i18n( "No steeping tea." ) );
    }
}


void TopLevel::setTeaList(const QList<Tea> &tealist) {
    m_tealist=tealist;

    // Save list...
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup tealistGroup( config, "Tealist" );

    tealistGroup.deleteGroup();

    for(int i=0; i<m_tealist.size(); ++i) {
        tealistGroup.writeEntry(QStringLiteral( "Tea%1 Time" ).arg( i ), m_tealist.at( i ).time() );
        tealistGroup.writeEntry(QStringLiteral( "Tea%1 Name" ).arg( i ), m_tealist.at( i ).name() );
    }
    tealistGroup.config()->sync();


    foreach(QAction* a, m_teaActionGroup->actions() ) {
        m_teaActionGroup->removeAction( a );
    }

    contextMenu()->clear();

    loadTeaMenuItems();
    contextMenu()->addSeparator();
    contextMenu()->addAction( m_actionCollection->action(QStringLiteral("stop")) );
    contextMenu()->addAction( m_actionCollection->action(QStringLiteral("anonymous")) );
    contextMenu()->addSeparator();
    contextMenu()->addAction( m_actionCollection->action(QStringLiteral("configure")) );
    contextMenu()->addAction( m_actionCollection->action(QLatin1String(KStandardAction::name(KStandardAction::ConfigureNotifications))) );
    contextMenu()->addMenu(m_helpMenu->menu() );
    contextMenu()->addSeparator();
    contextMenu()->addAction( m_actionCollection->action(QLatin1String(KStandardAction::name(KStandardAction::Quit))) );

    loadConfig();
}


void TopLevel::loadTeaMenuItems() {
    int i=0;

    foreach(const Tea &t, m_tealist) {
        QAction *a = contextMenu()->addAction(
                   i18nc( "%1 - name of the tea, %2 - the predefined time for "
                          "the tea", "%1 (%2)", t.name(), t.timeToString() )
                     );

        a->setData( ++i );
        m_teaActionGroup->addAction( a );
    }
}


void TopLevel::showSettingsDialog()
{
    SettingsDialog( this, m_tealist ).exec();
}


void TopLevel::showTimeEditDialog()
{
    TimeEditDialog( this ).exec();
}


void TopLevel::runTea(QAction *a)
{
    int index = a->data().toInt();
    if( index <= 0 ) {
        return;
    }

    --index;

    if( index > m_tealist.size() ) {
        return;
    }

    runTea( m_tealist.at( index ) );
}


void TopLevel::runTea(const Tea &tea)
{
    m_runningTea = tea;
    m_runningTeaTime = m_runningTea.time();

    checkState();
    repaintTrayIcon();

    m_timer->start( 1000 );
}


void TopLevel::repaintTrayIcon()
{
    if( m_runningTeaTime != 0 && m_usevisualize) {
        setOverlayIconByName ( QStringLiteral("task-ongoing") );
    }
    else
    {
        setOverlayIconByName ( QString() );
    }
}


void TopLevel::teaTimeEvent()
{
    QString title = i18n( "The Tea Cooker" );

    if( m_runningTeaTime == 0 ) {
        m_timer->stop();

        QString content = i18n( "%1 is now ready!", m_runningTea.name() );

        //NOTICE Timeout is set t ~o24 days when no auto hide is request. Ok - nearly the same...
        if( m_usepopup ) {
            showMessage( title, content, iconName(), m_autohide ? m_autohidetime*1000 : 2100000000 );
        }

        KNotification::event( QStringLiteral( "ready" ), content );

        if( m_usereminder && m_remindertime > 0 ) {
            setTooltipText( content );

            m_timer->start( 1000 );
            m_nextNotificationTime -= m_remindertime;
            --m_runningTeaTime;
        }
        checkState();
        repaintTrayIcon();
    }
    else if(m_runningTeaTime<0) {
        QString content = i18n( "%1 is ready since %2!", m_runningTea.name(), Tea::int2time( m_runningTeaTime*-1, true ) );
        setTooltipText( content );

        if( m_runningTeaTime == m_nextNotificationTime ) {
            if( m_usepopup ) {
                showMessage( title, content, iconName(), m_autohide ? m_autohidetime*1000 : 2100000000 );
            }

            KNotification::event( QLatin1String( "reminder" ), content );

            m_nextNotificationTime -= m_remindertime;
        }
        --m_runningTeaTime;
    }
    else {
        --m_runningTeaTime;
        setTooltipText( i18nc( "%1 is the time, %2 is the name of the tea", "%1 left for %2.", Tea::int2time( m_runningTeaTime, true ), m_runningTea.name() ) );
    }

    if( m_usevisualize ) {
        repaintTrayIcon();
    }
}


void TopLevel::cancelTea()
{
    m_timer->stop();
    m_runningTeaTime = 0;
    checkState();
    repaintTrayIcon();
}

void TopLevel::configureNotifications()
{
    KNotifyConfigWidget::configure();
}

void TopLevel::loadConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup generalGroup( config, "General" );

    m_usepopup=generalGroup.readEntry( "UsePopup", true );
    m_autohide=generalGroup.readEntry( "PopupAutoHide", false );
    m_autohidetime=generalGroup.readEntry( "PopupAutoHideTime", 30 );
    m_usereminder=generalGroup.readEntry( "UseReminder", false );
    m_remindertime=generalGroup.readEntry( "ReminderTime", 60 );
    m_usevisualize=generalGroup.readEntry( "UseVisualize", true );
}


void TopLevel::showPopup(bool active, const QPoint& point)
{
    Q_UNUSED( active )
    if ( m_popup->isVisible() )
    {
        m_popup->hide();
    }
    else
    {
        QPoint p = point;
        QRect r = QApplication::desktop()->screenGeometry( p );
        QSize popupSize = m_popup->minimumSizeHint();
        if ( p.x() + popupSize.width() > r.right() )
        {
            p.rx() -= popupSize.width();
        }
        if ( p.y() + popupSize.height() > r.bottom() )
        {
            p.ry() -= popupSize.height();
        }
        m_popup->show( p );
    }
}


void TopLevel::setTooltipText(const QString& content)
{
    const QString title = i18n( "The Tea Cooker" );
    setToolTip( iconName(), title, content );
    m_popup->setView( title, content, KIconLoader().loadIcon(iconName(), KIconLoader::MainToolbar) );
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
