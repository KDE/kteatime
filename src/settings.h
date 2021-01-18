/*
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
#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settings.h"

#include <QDialog>

class TopLevel;
class TeaListModel;
class Tea;
class SettingsUI;


/**
 * @short the settings dialog
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class SettingsDialog : public QDialog
{
    public:
        SettingsDialog(TopLevel *toplevel, const QList<Tea> &teas);
        ~SettingsDialog();

    private:
        void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

        void accept() override;
        void checkPopupButtonState(bool b);
        void showHelp();

        void newButtonClicked();
        void removeButtonClicked();
        void upButtonClicked();
        void downButtonClicked();

        void nameValueChanged(const QString &text);
        void timeValueChanged();

        void moveSelectedItem(bool moveup);

        SettingsUI *const mUi;
        TopLevel *const m_toplevel;
        TeaListModel *m_model = nullptr;
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
