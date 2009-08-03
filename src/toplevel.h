/*
 *   This file is part of the KTeaTime application.
 *
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *   Copyright (C) 2002-2003  Martin Willers <willers@xm-arts.de>
 *   Copyright (c) 2007       Stefan Böhmann <kde@hilefoks.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include "tea.h"

#include <QIcon>
#include <QPixmap>
#include <KSystemTrayIcon>


class QAction;
class QActionGroup;
class KHelpMenu;
class KPassivePopup;
class KAboutData;



/**
 * @short the main class for KTeatime
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TopLevel : public KSystemTrayIcon
{
    Q_OBJECT

    public:
        explicit TopLevel(const KAboutData *aboutData, const QString &icon="kteatime", QWidget *parent=0);
        ~TopLevel();
        void setTeaList(QList<Tea> tealist);
        void runTea(Tea tea);

    private:
        void checkState();
        void loadConfig();
        void loadTeaMenuItems();
        QPoint calculatePopupPoint();
        void repaintTrayIcon();

        QList<Tea> m_tealist;
        QAction *m_stopAct, *m_confAct, *m_anonAct, *m_exitAct;
        QActionGroup *m_teaActionGroup;

        KHelpMenu *m_helpMenu;
        QTimer *m_timer;
        KPassivePopup *m_popup;

        int m_runningTeaTime;
        int m_nextNotificationTime;
        Tea m_runningTea;

        /** should we use notifications defined by KNotification */
        bool m_usenotification;

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

        const QIcon m_icon;

        QPixmap m_pix;

    private slots:
	void runTea(QAction *a);
        void showSettingsDialog();
        void showTimeEditDialog();
        void teaTimeEvent();
        void cancelTea();
        void showPopup(QSystemTrayIcon::ActivationReason reason);
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
