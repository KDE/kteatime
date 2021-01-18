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
  : QDialog()
  , mUi(new TimeEditUI( this ))
  , mToplevel( toplevel )
{
    setWindowTitle( i18n( "Custom Tea" ) );

    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    mButtonBox->button(QDialogButtonBox::Ok)->setWhatsThis(i18n( "Start a new custom tea with the time configured in this dialog."  ));
    mButtonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    mButtonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::CTRL | Qt::Key_Return);
    mButtonBox->button(QDialogButtonBox::Cancel)->setWhatsThis(i18n( "Close this dialog without starting a new tea."  ));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mUi);
    mainLayout->addWidget(mButtonBox);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group( config, "AnonymousTeaDialog" );

    int time=group.readEntry( "AnonymousTeaTime", 180 );

    mUi->minutes->setSuffix( ki18np( " minute", " minutes") );
    mUi->seconds->setSuffix( ki18np( " second", " seconds") );

    mUi->minutes->setValue( time / 60 );
    mUi->seconds->setValue( time % 60 );

    mUi->minutes->setFocus( Qt::ShortcutFocusReason );

    restoreGeometry(group.readEntry<QByteArray>("Geometry", QByteArray()));

    const QSize desktopSize = qApp->primaryScreen()->size();
    int x = group.readEntry( "AnonymousTeaDialogXPos", desktopSize.width()/2 - width()/2 );
    int y = group.readEntry( "AnonymousTeaDialogYPos", desktopSize.height()/2 - height()/2 );

    x = qMin( qMax( 0, x ), desktopSize.width() - width() );
    x = qMin( qMax( 0, y ), desktopSize.height() - height() );
    move( QPoint( x, y ) );

    connect(mUi->minutes, QOverload<int>::of(&KPluralHandlingSpinBox::valueChanged), this, &TimeEditDialog::checkOkButtonState);
    connect(mUi->seconds, QOverload<int>::of(&KPluralHandlingSpinBox::valueChanged), this, &TimeEditDialog::checkOkButtonState);

    connect(mButtonBox, &QDialogButtonBox::accepted, this, &TimeEditDialog::accept);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &TimeEditDialog::reject);
}


TimeEditDialog::~TimeEditDialog()
{
    delete mUi;
}


void TimeEditDialog::checkOkButtonState()
{
    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled( mUi->minutes->value() || mUi->seconds->value() );
}


void TimeEditDialog::accept()
{
    hide();

    int time = mUi->seconds->value();
    time += mUi->minutes->value() * 60;

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group( config, "AnonymousTeaDialog" );
    group.writeEntry( "AnonymousTeaTime", time );
    group.writeEntry("Geometry", saveGeometry());

    group.writeEntry( "AnonymousTeaDialogXPos", x() );
    group.writeEntry( "AnonymousTeaDialogYPos", y() );

    mToplevel->runTea( Tea( i18n( "Custom Tea" ), time ) );
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
