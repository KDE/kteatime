/*
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "settings.h"

#include "toplevel.h"
#include "tealistmodel.h"

#include <QDialogButtonBox>
#include <QScreen>
#include <QHashIterator>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include <KConfigGroup>
#include <KHelpClient>
#include <KLocalizedString>
#include <KSharedConfig>

class SettingsUI : public QWidget, public Ui::SettingsWidget
{

    public:
        explicit SettingsUI(QWidget *parent = nullptr)
          : QWidget( parent )
        {
            setupUi( this );
        }
};

SettingsDialog::SettingsDialog(TopLevel *toplevel, const QList<Tea> &teas)
  : QDialog()
  , mUi(new SettingsUI( this ))
  , m_toplevel(toplevel)

{
    setWindowTitle( i18n( "Configure Tea Cooker" ) );

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Help, this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mUi);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::helpRequested, this, &SettingsDialog::showHelp);

    buttonBox->button(QDialogButtonBox::Ok)->setWhatsThis(i18n( "Save changes and close dialog."  ));
    buttonBox->button(QDialogButtonBox::Cancel)->setWhatsThis(i18n( "Close dialog without saving changes."  ));
    buttonBox->button(QDialogButtonBox::Help)->setWhatsThis(i18n( "Show help page for this dialog."  ));

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group( config, "General" );

    restoreGeometry(group.readEntry<QByteArray>("Geometry", QByteArray()));

    const QSize desktopSize = qApp->primaryScreen()->size();
    int x=group.readEntry( "SettingsDialogXPos", desktopSize.width()/2 - width()/2 );
    int y=group.readEntry( "SettingsDialogYPos", desktopSize.height()/2 - height()/2 );

    x = qMin( qMax( 0, x ), desktopSize.width() - width() );
    x = qMin( qMax( 0, y ), desktopSize.height() - height() );
    move( QPoint( x, y ) );

    bool popup=group.readEntry( "UsePopup", true );
    bool autohide=group.readEntry( "PopupAutoHide", false );
    int autohidetime=group.readEntry( "PopupAutoHideTime", 30 );
    bool reminder=group.readEntry( "UseReminder", false );
    int remindertime=group.readEntry( "ReminderTime", 60 );
    bool vis=group.readEntry( "UseVisualize", true );

    mUi->popupCheckBox->setChecked( popup );
    mUi->autohideCheckBox->setChecked( autohide );
    mUi->reminderCheckBox->setChecked( reminder );
    mUi->visualizeCheckBox->setChecked( vis );

    mUi->autohideSpinBox->setValue( autohidetime );
    mUi->reminderSpinBox->setValue( remindertime );
    mUi->autohideSpinBox->setSuffix( ki18ncp( "Auto hide popup after", " second", " seconds") );
    mUi->reminderSpinBox->setSuffix( ki18ncp( "Reminder every", " second", " seconds") );

    mUi->autohideCheckBox->setEnabled( popup );
    mUi->autohideSpinBox->setEnabled( autohide );
    mUi->reminderSpinBox->setEnabled( reminder );

    m_model=new TeaListModel( teas, this );
    mUi->tealistTreeView->setModel( m_model );

    connect(mUi->tealistTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SettingsDialog::updateSelection);

    mUi->removeButton->setEnabled( false );
    mUi->upButton->setEnabled( false );
    mUi->downButton->setEnabled( false );

    mUi->newButton->setIcon( QIcon::fromTheme( QLatin1String(  "list-add" ) ) );
    mUi->removeButton->setIcon( QIcon::fromTheme( QLatin1String(  "edit-delete" ) ) );
    mUi->upButton->setIcon( QIcon::fromTheme( QLatin1String(  "arrow-up" ) ) );
    mUi->downButton->setIcon( QIcon::fromTheme( QLatin1String(  "arrow-down" ) ) );

    connect(mUi->popupCheckBox, &QCheckBox::toggled, this, &SettingsDialog::checkPopupButtonState);

    connect(mUi->newButton, &QToolButton::clicked, this, &SettingsDialog::newButtonClicked);
    connect(mUi->removeButton, &QToolButton::clicked, this, &SettingsDialog::removeButtonClicked);
    connect(mUi->upButton, &QToolButton::clicked, this, &SettingsDialog::upButtonClicked);
    connect(mUi->downButton, &QToolButton::clicked, this, &SettingsDialog::downButtonClicked);

    connect(mUi->teaNameEdit, &QLineEdit::textChanged, this, &SettingsDialog::nameValueChanged);
    connect(mUi->minutesSpin, &QSpinBox::valueChanged, this, &SettingsDialog::timeValueChanged);
    connect(mUi->secondsSpin, &QSpinBox::valueChanged, this, &SettingsDialog::timeValueChanged);
}

SettingsDialog::~SettingsDialog()
{
    delete m_model;
    delete mUi;
}

void SettingsDialog::showHelp()
{
    KHelpClient::invokeHelp();
}

void SettingsDialog::accept()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group( config, "General" );
    group.writeEntry( "SettingsDialogXPos", x() );
    group.writeEntry( "SettingsDialogYPos", y() );

    hide();
    group.writeEntry("Geometry", saveGeometry());

    group.writeEntry( "UsePopup",          mUi->popupCheckBox->checkState() == Qt::Checked );
    group.writeEntry( "PopupAutoHide",     mUi->autohideCheckBox->checkState() == Qt::Checked );
    group.writeEntry( "PopupAutoHideTime", mUi->autohideSpinBox->value() );
    group.writeEntry( "UseReminder",       mUi->reminderCheckBox->checkState() == Qt::Checked );
    group.writeEntry( "ReminderTime",      mUi->reminderSpinBox->value() );
    group.writeEntry( "UseVisualize",      mUi->visualizeCheckBox->checkState() == Qt::Checked );

    config->sync();
    m_toplevel->setTeaList( m_model->getTeaList() );
}


void SettingsDialog::checkPopupButtonState(bool b) {
    mUi->autohideCheckBox->setEnabled( b );

    if( !b ) {
        mUi->autohideSpinBox->setEnabled( b );
    }
    else if( mUi->autohideCheckBox->checkState() == 2 ) {
        mUi->autohideSpinBox->setEnabled( b );
    }
}

void SettingsDialog::newButtonClicked()
{
    int count = m_model->rowCount();
    m_model->insertRows( count, 1 );

    QItemSelectionModel *sm = mUi->tealistTreeView->selectionModel();
    QItemSelection selection( m_model->index( count, 0 ), m_model->index( count, 1 ) );
    sm->select( selection, QItemSelectionModel::Clear | QItemSelectionModel::Select );
}


void SettingsDialog::removeButtonClicked()
{
    const QModelIndexList indexes = mUi->tealistTreeView->selectionModel()->selectedIndexes();

    for (const QModelIndex &index : indexes) {
        // Only delete a row when column==0, otherwise the row will be delete
        // multiple times (the loop iterate over every cell, not over rows).
        if( index.column() == 0 ) {
            m_model->removeRows( index.row(), 1 );
        }
    }
}


void SettingsDialog::upButtonClicked()
{
    moveSelectedItem( true );
}


void SettingsDialog::downButtonClicked()
{
    moveSelectedItem( false );
}


void SettingsDialog::moveSelectedItem(bool moveup)
{
    QItemSelectionModel *sm = mUi->tealistTreeView->selectionModel();
    QModelIndexList items = sm->selection().indexes();

    if( !items.isEmpty() ) {
        QString name = m_model->data( m_model->index( items.at(0).row(), 0 ), Qt::EditRole).toString();
        unsigned time = m_model->data( m_model->index( items.at(0).row(), 1 ), Qt::EditRole).toUInt();
        int pos = items.at(0).row();

        moveup ? --pos : ++pos;

        removeButtonClicked();

        m_model->insertRows( pos, 1 );
        m_model->setData( m_model->index( pos, 0 ), name, Qt::EditRole );
        m_model->setData( m_model->index( pos, 1 ), time, Qt::EditRole );

        QItemSelection selection( m_model->index( pos, 0 ), m_model->index( pos, 1 ) );
        sm->select( selection, QItemSelectionModel::Clear | QItemSelectionModel::Select );
    }
}


void SettingsDialog::updateSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected)
    QModelIndexList items = selected.indexes();

    QString name;
    unsigned time=0;

    bool state = !items.isEmpty();

    mUi->teaPropertiesGroup->setEnabled( state );
    mUi->teaNameEdit->setEnabled( state );
    mUi->minutesSpin->setEnabled( state );
    mUi->secondsSpin->setEnabled( state );
    mUi->removeButton->setEnabled( state );

    if( state ) {
        name = m_model->data( m_model->index( items.at(0).row(), 0 ), Qt::EditRole ).toString();
        time = m_model->data( m_model->index( items.at(0).row(), 1 ), Qt::EditRole ).toUInt();

        mUi->upButton->setEnabled( items.at(0).row() > 0 );
        mUi->downButton->setEnabled( items.at(0).row() < ( m_model->rowCount() - 1 ) );
    }
    else {
        mUi->upButton->setEnabled( false );
        mUi->downButton->setEnabled( false );
    }

    mUi->teaNameEdit->setText( name );
    mUi->minutesSpin->setValue( time / 60 );
    mUi->secondsSpin->setValue( time % 60 );
}


void SettingsDialog::timeValueChanged()
{
    QModelIndexList items = mUi->tealistTreeView->selectionModel()->selection().indexes();

    if( !items.isEmpty() ) {
        int time = mUi->secondsSpin->value();
        time += mUi->minutesSpin->value() * 60;

        if( time <= 0 ) {
            time = 1;
            mUi->secondsSpin->setValue( time );
        }
        m_model->setData( m_model->index( items.at(0).row(), 1 ), time, Qt::EditRole );
    }
}


void SettingsDialog::nameValueChanged(const QString &text)
{
    QModelIndexList items = mUi->tealistTreeView->selectionModel()->selection().indexes();

    if( !items.isEmpty() ) {
        m_model->setData( m_model->index( items.at(0).row(), 0 ), text, Qt::EditRole );
    }
}

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
