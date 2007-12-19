/*
 *   KTeaTime - A tea timer.
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *   Copyright (C) 2002-2003  Martin Willers <willers@xm-arts.de>
 *   Copyright (c) 2003 Daniel Teske <teske@bigfoot.com>
 *   Copyright (c) 2007 Stefan Böhmann <kde@hilefoks.org>
 *
 *   With contributions from Daniel Teske <teske@bigfoot.com>, and
 *   Jackson Dunstan <jdunstan@digipen.edu>
 *   (and possibly others, as well)
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

#include "timeedit.h"
#include "toplevel.h"
#include "tea.h"

#include <QDesktopWidget>


TimeEditUI::TimeEditUI(QWidget *parent): QFrame(parent)
{
    setupUi(this);
}


TimeEditDialog::TimeEditDialog(TopLevel *toplevel): KDialog(), m_toplevel(toplevel)
{
    setCaption(i18n("Anonymous Tea"));

    setButtonWhatsThis(KDialog::Ok, i18n("Start a new anonymous tea with the time configured in this dialog."));
    setButtonWhatsThis(KDialog::Cancel, i18n("Close this dialog without starting a new tea."));

    m_ui = new TimeEditUI(this);
    setMainWidget(m_ui);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "AnonymousTeaDialog");

    int time=group.readEntry("AnonymousTeaTime", 180);

    m_ui->minutes->setValue((time%(60*60))/60);
    m_ui->seconds->setValue(time%60);

    restoreDialogSize(group);

    QDesktopWidget desktop;
    int x=group.readEntry("AnonymousTeaDialogXPos", desktop.screenGeometry().width()/2 - width()/2 );
    int y=group.readEntry("AnonymousTeaDialogYPos", desktop.screenGeometry().height()/2 - height()/2 );

    if(x<0) x=0;
    else if(x>desktop.screenGeometry().width()-width()) x=desktop.screenGeometry().width()-width();

    if(y<0) y=0;
    else if(y>desktop.screenGeometry().height()-height()) y=desktop.screenGeometry().height()-height();

    move(QPoint(x,y));

    connect( m_ui->minutes, SIGNAL(valueChanged(int)), this, SLOT(checkOkButtonState()) );
    connect( m_ui->seconds, SIGNAL(valueChanged(int)), this, SLOT(checkOkButtonState()) );
}


TimeEditDialog::~TimeEditDialog()
{
    delete m_ui;
}


void TimeEditDialog::checkOkButtonState()
{
    enableButtonOk( m_ui->minutes->value() || m_ui->seconds->value() );
}


void TimeEditDialog::accept()
{
    hide();

    int time=m_ui->seconds->value();
    time+=m_ui->minutes->value()*60;

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "AnonymousTeaDialog");
    group.writeEntry("AnonymousTeaTime", time);
    saveDialogSize(group);

    group.writeEntry("AnonymousTeaDialogXPos", x());
    group.writeEntry("AnonymousTeaDialogYPos", y());

    m_toplevel->runTea(Tea(i18n("Anonymous Tea"), m_ui->minutes->value()*60 + m_ui->seconds->value()) );
}

