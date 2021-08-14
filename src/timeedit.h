/*
    SPDX-FileCopyrightText: 1998-1999 Matthias Hölzer-Klüpfel <matthias@hoelzer-kluepfel.de>
    SPDX-FileCopyrightText: 2002-2003 Martin Willers <willers@xm-arts.de>
    SPDX-FileCopyrightText: 2003 Daniel Teske <teske@bigfoot.com>
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
