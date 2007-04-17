/*
 *   KTeaTime - A tea timer.
 *
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel (hoelzer@kde.org)
 *   Copyright (C) 2002-2003  Martin Willers (willers@xm-arts.de)
 *   Copyright (C) 2003       Daniel Teske (teske@bigfoot.com)
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
 *
 */

#include "timeedit.h"
#include "timeedit.moc"

#include <klocale.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QHBoxLayout>

WrappingSpinBox::WrappingSpinBox(int minimum, int maximum, int step, QWidget *parent, const char *name)
	: QSpinBox(minimum, maximum, step, parent, name)
{
}

WrappingSpinBox::~WrappingSpinBox()
{
}


/** Overloaded QSpinBox method */
void WrappingSpinBox::stepUp()
{
	bool wrap = false;
	if (value() == 59)
		wrap = true;
	if (wrap)
		emit wrapUp();              // must wrap first (to avoid double-step-up)
	QSpinBox::stepUp();
}

/** Overloaded QSpinBox method */
void WrappingSpinBox::stepDown()
{
	bool wrap = false;
	if (value() == 0)
		wrap = true;
	QSpinBox::stepDown();
	if (wrap)
		emit wrapDown();
}


// -------------------------------------------------------------------------


TimeEdit::TimeEdit(QWidget* parent)
    : QWidget(parent)
{
	layout = new QHBoxLayout(this);
	minuteBox = new QSpinBox(0, 300, 1, this);
//	minuteBox->setFixedSize(minuteBox->sizeHint());

	QLabel* min = new QLabel(i18n(" min"), this);
	min->setFixedSize(min->sizeHint());
	secondBox = new WrappingSpinBox(0, 59, 1, this);
	secondBox->setWrapping(true);
//	secondBox->setFixedSize(secondBox->sizeHint());

	QLabel* sec = new QLabel(i18n(" sec"),this);
	sec->setFixedSize(sec->sizeHint());

	layout->addWidget(minuteBox);
	layout->addWidget(min);

	layout->addWidget(secondBox);
	layout->addWidget(sec);

	connect(minuteBox, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)) );
	connect(secondBox, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)) );
	connect(secondBox, SIGNAL(wrapUp()), SLOT(wrappedUp()));
	connect(secondBox, SIGNAL(wrapDown()), SLOT(wrappedDown()));
}

TimeEdit::~TimeEdit()
{
}

/** Set to specified number of seconds. */
void TimeEdit::setValue(int val)
{
	if (val < 0)
		return;

	// block signals to avoid receiption of valueChanged()
	// between changing of minutes and seconds
	secondBox->blockSignals(true);
	minuteBox->blockSignals(true);

	secondBox->setValue(val % 60);
	minuteBox->setValue(val / 60);

	secondBox->blockSignals(false);
	minuteBox->blockSignals(false);

	emit valueChanged(value());
}

/** Return current value in seconds. */
int TimeEdit::value()
{
	return minuteBox->value()*60 + secondBox->value();
}

/** SLOT: Handle wrap-up of seconds-box */
void TimeEdit::wrappedUp()
{
	if (minuteBox->value() != minuteBox->maximum()) {
		minuteBox->stepUp();
	} else {
		secondBox->setValue(58);    // ugly: must cater for wrapping-first
	}
}

/** SLOT: Handle wrap-down of seconds-box */
void TimeEdit::wrappedDown()
{
	// well, the "if" should always be true
	if (minuteBox->value() != minuteBox->minimum()) {
		minuteBox->stepDown();
	} else {
		secondBox->setValue(0);
	}
}

/** SLOT: Handle any change in minutes of seconds */
void TimeEdit::spinBoxValueChanged(int)
{
	if (value() == 0) {
		secondBox->stepUp();        // this will give another spinBoxValueChanged() invocation
		return;
	}

	emit valueChanged(value());
}

/** SLOT (overloading QSpinBox): set focus */
void TimeEdit::setFocus()
{
	minuteBox->setFocus();
}
