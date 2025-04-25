/*
    SPDX-FileCopyrightText: 1998-1999 Matthias Hölzer-Klüpfel <matthias@hoelzer-kluepfel.de>
    SPDX-FileCopyrightText: 2002-2003 Martin Willers <willers@xm-arts.de>
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include "tea.h"

#include <QPointer>
#include <KStatusNotifierItem>

class QAction;
class QActionGroup;
class QTimer;
class KAboutData;
class KActionCollection;
class KHelpMenu;
class KNotification;

/**
 * @short the main class for KTeatime
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TopLevel : public KStatusNotifierItem
{
public:
    explicit TopLevel(const KAboutData *aboutData,
                      const QString &trayIcon = QLatin1String("kteatime-symbolic"),
                      const QString &notificationIcon = QLatin1String("kteatime"),
                      QWidget *parent = nullptr);
    ~TopLevel() override;
    void setTeaList(const QList<Tea> &tealist);
    void runTea(const Tea &tea);

private:
    void slotRunTea(QAction *a);
    void showSettingsDialog();
    void showTimeEditDialog();
    void teaTimeEvent();
    void cancelTea();
    void stopTea();
    void resumeTea();
    void showPopup();
    void checkState();
    void loadConfig();
    void loadTeaMenuItems();
    void repaintTrayIcon();
    void setTooltipText(const QString &content);
    void configureNotifications();

    QList<Tea> m_tealist;
    QAction *action = nullptr;
    QActionGroup *m_teaActionGroup = nullptr;
    KActionCollection *m_actionCollection = nullptr;

    KHelpMenu *m_helpMenu = nullptr;
    QTimer *m_timer = nullptr;
    QPointer<KNotification> m_popup;
    QString m_trayIconName;
    QString m_notificationIconName;

    int m_runningTeaTime;
    int m_pausedRemainingTeaTime;
    int m_nextNotificationTime;
    Tea m_runningTea;

    /** should we show a popup for events */
    bool m_usepopup;

    /** auto hide the popup? */
    bool m_autohide;

    /** time after the popup should be hide. */
    int m_autohidetime;

    /** remind us about a ready tea? */
    bool m_usereminder;

    /** the time bedween remind events */
    int m_remindertime;

    /** use a visual effect in the system tray icon. */
    bool m_usevisualize;
};

#endif
