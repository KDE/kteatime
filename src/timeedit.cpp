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
#include "timeedit.h"
#include "toplevel.h"
#include "tea.h"

#include <QScreen>
#include <QGuiApplication>
#include <QDialogButtonBox>
#include <QPushButton>

#include <KConfigGroup>
#include <KSharedConfig>


class TimeEditUI : public QWidget, public Ui::TimeEditWidget
{
    public:
        explicit TimeEditUI(QWidget *parent = nullptr)
          : QWidget( parent )
        {
            setupUi( this );
        }
};


TimeEditDialog::TimeEditDialog(TopLevel *toplevel)
  : QDialog(),
    m_toplevel( toplevel )
{
    setWindowTitle( i18n( "Anonymous Tea" ) );

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setWhatsThis(i18n( "Start a new anonymous tea with the time configured in this dialog."  ));
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::CTRL | Qt::Key_Return);
    buttonBox->button(QDialogButtonBox::Cancel)->setWhatsThis(i18n( "Close this dialog without starting a new tea."  ));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    ui = new TimeEditUI( this );
    mainLayout->addWidget(ui);
    mainLayout->addWidget(buttonBox);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group( config, "AnonymousTeaDialog" );

    int time=group.readEntry( "AnonymousTeaTime", 180 );

    ui->minutes->setSuffix( ki18np( " minute", " minutes") );
    ui->seconds->setSuffix( ki18np( " second", " seconds") );

    ui->minutes->setValue( time / 60 );
    ui->seconds->setValue( time % 60 );

    ui->minutes->setFocus( Qt::ShortcutFocusReason );

    restoreGeometry(group.readEntry<QByteArray>("Geometry", QByteArray()));

    const QSize desktopSize = qApp->primaryScreen()->size();
    int x = group.readEntry( "AnonymousTeaDialogXPos", desktopSize.width()/2 - width()/2 );
    int y = group.readEntry( "AnonymousTeaDialogYPos", desktopSize.height()/2 - height()/2 );

    x = qMin( qMax( 0, x ), desktopSize.width() - width() );
    x = qMin( qMax( 0, y ), desktopSize.height() - height() );
    move( QPoint( x, y ) );

    connect(ui->minutes, QOverload<int>::of(&KPluralHandlingSpinBox::valueChanged), this, &TimeEditDialog::checkOkButtonState);
    connect(ui->seconds, QOverload<int>::of(&KPluralHandlingSpinBox::valueChanged), this, &TimeEditDialog::checkOkButtonState);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &TimeEditDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &TimeEditDialog::reject);
}


TimeEditDialog::~TimeEditDialog()
{
    delete ui;
}


void TimeEditDialog::checkOkButtonState()
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled( ui->minutes->value() || ui->seconds->value() );
}


void TimeEditDialog::accept()
{
    hide();

    int time = ui->seconds->value();
    time += ui->minutes->value() * 60;

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group( config, "AnonymousTeaDialog" );
    group.writeEntry( "AnonymousTeaTime", time );
    group.writeEntry("Geometry", saveGeometry());

    group.writeEntry( "AnonymousTeaDialogXPos", x() );
    group.writeEntry( "AnonymousTeaDialogYPos", y() );

    m_toplevel->runTea( Tea( i18n( "Anonymous Tea" ), time ) );
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
