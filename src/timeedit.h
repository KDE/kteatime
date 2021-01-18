/*
 * Copyright 1998-1999 by Matthias Hölzer-Klüpfel <matthias@hoelzer-kluepfel.de>
 * Copyright 2002-2003 by Martin Willers <willers@xm-arts.de>
 * Copyright 2003      by Daniel Teske <teske@bigfoot.com>
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
#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include "ui_timeedit.h"

#include <QDialog>

class QDialogButtonBox;
class TopLevel;
class TimeEditUI;


/**
 * @short the timeedit dialog.
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TimeEditDialog : public QDialog
{
    public:
        explicit TimeEditDialog(TopLevel *toplevel);
        ~TimeEditDialog() override;

    private:
        void checkOkButtonState();
        void accept() override;

        TimeEditUI *const mUi;
        TopLevel *const mToplevel;
        QDialogButtonBox *mButtonBox = nullptr;
};

#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
