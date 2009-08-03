/*
 *   Copyright (c) 2007  Stefan Böhmann <kde@hilefoks.org>
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

#include "settings.h"
#include "toplevel.h"
#include "tealistmodel.h"

#include <QHashIterator>
#include <QString>
#include <QDesktopWidget>

#include <knotifyconfigwidget.h>


SettingsUI::SettingsUI(QWidget *parent): QFrame(parent)
{
    setupUi(this);
}



SettingsDialog::SettingsDialog(TopLevel *toplevel, const QList<Tea> &teas): KDialog(), m_toplevel(toplevel)
{
    setCaption(i18n("Configure Tea Cooker"));

    setButtons(Help | Ok | Cancel);
    setHelp("configure");

    setButtonWhatsThis(KDialog::Ok, i18n("Save changes and close dialog."));
    setButtonWhatsThis(KDialog::Cancel, i18n("Close dialog without saving changes."));
    setButtonWhatsThis(KDialog::Help, i18n("Show help page for this dialog."));

    m_ui = new SettingsUI(this);
    setMainWidget(m_ui);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "General");

    restoreDialogSize(group);

    QDesktopWidget desktop;
    int x=group.readEntry("SettingsDialogXPos", desktop.screenGeometry().width()/2 - width()/2 );
    int y=group.readEntry("SettingsDialogYPos", desktop.screenGeometry().height()/2 - height()/2 );

    if(x<0) x=0;
    else if(x>desktop.screenGeometry().width()-width()) x=desktop.screenGeometry().width()-width();

    if(y<0) y=0;
    else if(y>desktop.screenGeometry().height()-height()) y=desktop.screenGeometry().height()-height();

    move(QPoint(x,y));

    bool noti=group.readEntry("UseNotification", true);
    bool popup=group.readEntry("UsePopup", true);
    bool autohide=group.readEntry("PopupAutoHide", false);
    int autohidetime=group.readEntry("PopupAutoHideTime", 30);
    bool reminder=group.readEntry("UseReminder", false);
    int remindertime=group.readEntry("ReminderTime", 60);
    bool vis=group.readEntry("UseVisualize", true);

    m_ui->notificationCheckBox->setChecked(noti?Qt::Checked:Qt::Unchecked);
    m_ui->popupCheckBox->setChecked(popup?Qt::Checked:Qt::Unchecked);
    m_ui->autohideCheckBox->setChecked(autohide?Qt::Checked:Qt::Unchecked);
    m_ui->reminderCheckBox->setChecked(reminder?Qt::Checked:Qt::Unchecked);
    m_ui->visualizeCheckBox->setChecked(vis?Qt::Checked:Qt::Unchecked);

    m_ui->autohideSpinBox->setValue(autohidetime);
    m_ui->reminderSpinBox->setValue(remindertime);
    updateSpinBoxSuffix();

    m_ui->notificationButton->setEnabled(noti);
    m_ui->autohideCheckBox->setEnabled(popup);
    m_ui->autohideSpinBox->setEnabled(autohide);
    m_ui->reminderSpinBox->setEnabled(reminder);

    m_model=new TeaListModel(teas, this);
    m_ui->tealistTreeView->setModel(m_model);
    connect(m_ui->tealistTreeView->selectionModel(),
              SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this,
              SLOT(updateSelection(const QItemSelection&, const QItemSelection&))
      );

    m_ui->removeButton->setEnabled(false);
    m_ui->upButton->setEnabled(false);
    m_ui->downButton->setEnabled(false);

    m_ui->newButton->setIcon(KIcon("list-add"));
    m_ui->removeButton->setIcon(KIcon("edit-delete"));
    m_ui->upButton->setIcon(KIcon("arrow-up"));
    m_ui->downButton->setIcon(KIcon("arrow-down"));

    connect(m_ui->popupCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkPopupButtonState(bool)) );
    connect(m_ui->notificationButton, SIGNAL(clicked()), this, SLOT(confButtonClicked()) );

    connect(m_ui->newButton, SIGNAL(clicked()), this, SLOT(newButtonClicked()) );
    connect(m_ui->removeButton, SIGNAL(clicked()), this, SLOT(removeButtonClicked()) );
    connect(m_ui->upButton, SIGNAL(clicked()), this, SLOT(upButtonClicked()) );
    connect(m_ui->downButton, SIGNAL(clicked()), this, SLOT(downButtonClicked()) );

    connect(m_ui->teaNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(nameValueChanged(const QString&)) );
    connect(m_ui->minutesSpin, SIGNAL(valueChanged(int)), this, SLOT(timeValueChanged()) );
    connect(m_ui->secondsSpin, SIGNAL(valueChanged(int)), this, SLOT(timeValueChanged()) );
    connect(m_ui->autohideSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateSpinBoxSuffix()) );
    connect(m_ui->reminderSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateSpinBoxSuffix()) );
}


SettingsDialog::~SettingsDialog()
{
    delete m_model;
    delete m_ui;
}


void SettingsDialog::accept()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "General");
    group.writeEntry("SettingsDialogXPos", x());
    group.writeEntry("SettingsDialogYPos", y());

    hide();
    saveDialogSize(group);

    group.writeEntry("UseNotification",  m_ui->notificationCheckBox->checkState()==Qt::Checked);
    group.writeEntry("UsePopup",         m_ui->popupCheckBox->checkState()==Qt::Checked);
    group.writeEntry("PopupAutoHide",    m_ui->autohideCheckBox->checkState()==Qt::Checked);
    group.writeEntry("PopupAutoHideTime",m_ui->autohideSpinBox->value());
    group.writeEntry("UseReminder",      m_ui->reminderCheckBox->checkState()==Qt::Checked);
    group.writeEntry("ReminderTime",     m_ui->reminderSpinBox->value());
    group.writeEntry("UseVisualize",     m_ui->visualizeCheckBox->checkState()==Qt::Checked);

    config->sync();
    m_toplevel->setTeaList(m_model->getTeaList());
}


void SettingsDialog::checkPopupButtonState(bool b) {
    m_ui->autohideCheckBox->setEnabled(b);
    if(!b)
        m_ui->autohideSpinBox->setEnabled(b);
    else if(m_ui->autohideCheckBox->checkState()==2)
        m_ui->autohideSpinBox->setEnabled(b);
}


void SettingsDialog::confButtonClicked()
{
    KNotifyConfigWidget::configure(this);
}


void SettingsDialog::newButtonClicked()
{
    int count=m_model->rowCount();
    m_model->insertRows(count, 1);
    QItemSelectionModel *sm=m_ui->tealistTreeView->selectionModel();
    QItemSelection selection(m_model->index(count, 0), m_model->index(count, 1));
    sm->select(selection, QItemSelectionModel::Clear|QItemSelectionModel::Select);
}


void SettingsDialog::removeButtonClicked()
{
    QModelIndexList indexes=m_ui->tealistTreeView->selectionModel()->selectedIndexes();
    foreach(const QModelIndex &index, indexes) {
        // Only delete a row when column==0, otherwise the row will be delete
        // multiple times (the loop iterate over every cell, not over rows).
        if(index.column()==0)
            m_model->removeRows(index.row(), 1);
    }
}


void SettingsDialog::upButtonClicked()
{
    moveSelectedItem(true);
}


void SettingsDialog::downButtonClicked()
{
    moveSelectedItem(false);
}


void SettingsDialog::moveSelectedItem(bool moveup)
{
    QItemSelectionModel *sm=m_ui->tealistTreeView->selectionModel();
    QModelIndexList items = sm->selection().indexes();
    if(!items.isEmpty()) {
        QString name=m_model->data(m_model->index(items.at(0).row(), 0), Qt::EditRole).toString();
        unsigned time=m_model->data(m_model->index(items.at(0).row(), 1), Qt::EditRole).toUInt();
        int pos=items.at(0).row();

        moveup?--pos:++pos;

        removeButtonClicked();
        m_model->insertRows(pos, 1);
        m_model->setData(m_model->index(pos, 0), name, Qt::EditRole);
        m_model->setData(m_model->index(pos, 1), time, Qt::EditRole);

        QItemSelection selection(m_model->index(pos, 0), m_model->index(pos, 1));
        sm->select(selection, QItemSelectionModel::Clear|QItemSelectionModel::Select);
    }
}


void SettingsDialog::updateSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList items = selected.indexes();

    QString name;
    unsigned time=0;

    bool state=!items.isEmpty();

    m_ui->teaPropertiesGroup->setEnabled(state);
    m_ui->teaNameEdit->setEnabled(state);
    m_ui->minutesSpin->setEnabled(state);
    m_ui->secondsSpin->setEnabled(state);
    m_ui->removeButton->setEnabled(state);

    if(state) {
        name=m_model->data(m_model->index(items.at(0).row(), 0), Qt::EditRole).toString();
        time=m_model->data(m_model->index(items.at(0).row(), 1), Qt::EditRole).toUInt();

        m_ui->upButton->setEnabled(items.at(0).row()>0);
        m_ui->downButton->setEnabled( items.at(0).row() < (m_model->rowCount()-1) );
    }
    else {
        m_ui->upButton->setEnabled(false);
        m_ui->downButton->setEnabled(false);
    }
    m_ui->teaNameEdit->setText(name);
    m_ui->minutesSpin->setValue((time%(60*60))/60);
    m_ui->secondsSpin->setValue(time%60);
}


void SettingsDialog::timeValueChanged()
{
    QModelIndexList items = m_ui->tealistTreeView->selectionModel()->selection().indexes();
    if(!items.isEmpty()) {
        int time=m_ui->secondsSpin->value();
        time+=m_ui->minutesSpin->value()*60;

        if(time<=0) {
            time=1;
            m_ui->secondsSpin->setValue(time);
        }
        m_model->setData(m_model->index(items.at(0).row(), 1), time, Qt::EditRole);
    }
}


void SettingsDialog::nameValueChanged(const QString &text)
{
    QModelIndexList items = m_ui->tealistTreeView->selectionModel()->selection().indexes();
    if(!items.isEmpty())
        m_model->setData(m_model->index(items.at(0).row(), 0), text, Qt::EditRole);
}


void SettingsDialog::updateSpinBoxSuffix()
{
    m_ui->autohideSpinBox->setSuffix(i18ncp("Auto hide popup after", " second", " seconds", m_ui->autohideSpinBox->value()));
    m_ui->reminderSpinBox->setSuffix(i18ncp("Reminder every", " second", " seconds", m_ui->reminderSpinBox->value()));
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
