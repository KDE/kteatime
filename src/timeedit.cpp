/*
    SPDX-FileCopyrightText: 1998-1999 Matthias Hölzer-Klüpfel <matthias@hoelzer-kluepfel.de>
    SPDX-FileCopyrightText: 2002-2003 Martin Willers <willers@xm-arts.de>
    SPDX-FileCopyrightText: 2003 Daniel Teske <teske@bigfoot.com>
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "timeedit.h"
#include "tea.h"
#include "toplevel.h"

#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QPushButton>
#include <QScreen>

#include <KConfigGroup>
#include <KSharedConfig>

class TimeEditUI : public QWidget, public Ui::TimeEditWidget
{
public:
    explicit TimeEditUI(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

TimeEditDialog::TimeEditDialog(TopLevel *toplevel)
    : QDialog()
    , mUi(new TimeEditUI(this))
    , mToplevel(toplevel)
{
    setWindowTitle(i18n("Custom Tea"));

    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mButtonBox->button(QDialogButtonBox::Ok)->setWhatsThis(i18n("Start a new custom tea with the time configured in this dialog."));
    mButtonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    mButtonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::CTRL | Qt::Key_Return);
    mButtonBox->button(QDialogButtonBox::Cancel)->setWhatsThis(i18n("Close this dialog without starting a new tea."));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mUi);
    mainLayout->addWidget(mButtonBox);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QStringLiteral("AnonymousTeaDialog"));

    int time = group.readEntry("AnonymousTeaTime", 180);

    mUi->minutes->setSuffix(ki18np(" minute", " minutes"));
    mUi->seconds->setSuffix(ki18np(" second", " seconds"));

    mUi->minutes->setValue(time / 60);
    mUi->seconds->setValue(time % 60);

    mUi->minutes->setFocus(Qt::ShortcutFocusReason);

    restoreGeometry(group.readEntry<QByteArray>("Geometry", QByteArray()));

    const QSize desktopSize = qApp->primaryScreen()->size();
    int x = group.readEntry("AnonymousTeaDialogXPos", desktopSize.width() / 2 - width() / 2);
    int y = group.readEntry("AnonymousTeaDialogYPos", desktopSize.height() / 2 - height() / 2);

    x = qMin(qMax(0, x), desktopSize.width() - width());
    x = qMin(qMax(0, y), desktopSize.height() - height());
    move(QPoint(x, y));

    connect(mUi->minutes, &KPluralHandlingSpinBox::valueChanged, this, &TimeEditDialog::checkOkButtonState);
    connect(mUi->seconds, &KPluralHandlingSpinBox::valueChanged, this, &TimeEditDialog::checkOkButtonState);

    connect(mButtonBox, &QDialogButtonBox::accepted, this, &TimeEditDialog::accept);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &TimeEditDialog::reject);
}

TimeEditDialog::~TimeEditDialog()
{
    delete mUi;
}

void TimeEditDialog::checkOkButtonState()
{
    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(mUi->minutes->value() || mUi->seconds->value());
}

void TimeEditDialog::accept()
{
    hide();

    int time = mUi->seconds->value();
    time += mUi->minutes->value() * 60;

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QStringLiteral("AnonymousTeaDialog"));
    group.writeEntry("AnonymousTeaTime", time);
    group.writeEntry("Geometry", saveGeometry());

    group.writeEntry("AnonymousTeaDialogXPos", x());
    group.writeEntry("AnonymousTeaDialogYPos", y());

    mToplevel->runTea(Tea(i18n("Custom Tea"), time));
}

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on;
// replace-tabs on; space-indent on; vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
