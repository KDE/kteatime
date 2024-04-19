/*
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
    ~SettingsDialog() override;

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
