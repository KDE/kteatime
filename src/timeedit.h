/*
 *   KTeaTime - A tea timer.
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *   Copyright (C) 2002-2003  Martin Willers <willers@xm-arts.de>
 *   Copyright (c) 2003       Daniel Teske <teske@bigfoot.com>
 *   Copyright (c) 2007-2009  Stefan Böhmann <kde@hilefoks.org>
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

#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include "ui_timeedit.h"

class TopLevel;



/**
 * @short Class for wrapping the ui file for the timeedit dialog.
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TimeEditUI : public QFrame, public Ui::TimeEditWidget
{
    Q_OBJECT
    public:
        TimeEditUI(QWidget *parent = 0);
};


/**
 * @short the timeedit dialog.
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TimeEditDialog : public KDialog
{
    Q_OBJECT

    public:
        TimeEditDialog(TopLevel *toplevel);
        ~TimeEditDialog();

    private slots:
        void checkOkButtonState();
        void accept();

    private:
        TimeEditUI *ui;
        TopLevel *m_toplevel;
};

#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
