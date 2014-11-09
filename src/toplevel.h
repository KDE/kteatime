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
#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include "tea.h"

#include <KStatusNotifierItem>


class QAction;
class QActionGroup;
class QTimer;
class KAboutData;
class KActionCollection;
class KHelpMenu;
class KPassivePopup;

/**
 * @short the main class for KTeatime
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TopLevel : public KStatusNotifierItem
{
    Q_OBJECT
    public:
        explicit TopLevel(const KAboutData *aboutData, const QString &icon = QLatin1String("kteatime"), QWidget *parent = 0);
        ~TopLevel();
        void setTeaList(const QList<Tea> &tealist);
        void runTea(const Tea &tea);

    private:
        void runTea(QAction *a);
        void showSettingsDialog();
        void showTimeEditDialog();
        void teaTimeEvent();
        void cancelTea();
        void showPopup(bool active, const QPoint& point);
        void checkState();
        void loadConfig();
        void loadTeaMenuItems();
        void repaintTrayIcon();
        void setTooltipText(const QString& content);
    private Q_SLOTS:
        void configureNotifications();

    private:
        QList<Tea> m_tealist;
        QAction *action;
        QActionGroup *m_teaActionGroup;
        KActionCollection *m_actionCollection;

        KHelpMenu *m_helpMenu;
        QTimer *m_timer;
        KPassivePopup *m_popup;

        int m_runningTeaTime;
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

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
