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

#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <qspinbox.h>
#include <qwidget.h>
#include <QBoxLayout>

class QBoxLayout;


/**
 * @short   A spinbox that wraps around after reaching minimum resp. maximum.
 * @author  Daniel Teske
 */
class WrappingSpinBox : public QSpinBox
{
	Q_OBJECT

public:
	WrappingSpinBox(int minimum, int maximum, int step = 1, QWidget *parent=0, const char *name=0);
	~WrappingSpinBox();

	void stepUp();
	void stepDown();

signals:
	void wrapUp();
	void wrapDown();
};


/**
 * @short   A widget for entering a timeout in minutes and seconds.
 * @author  Daniel Teske
 */
class TimeEdit : public QWidget
{
	Q_OBJECT

public:
	explicit TimeEdit(QWidget* parent = 0);
	~TimeEdit();

	void setValue(int value);
	int value();

public slots:
	void setFocus();

private slots:
	void spinBoxValueChanged(int);
	void wrappedUp();
	void wrappedDown();

signals:
	void valueChanged(int);


protected:
	QSpinBox *minuteBox;
	WrappingSpinBox *secondBox;
	QBoxLayout* layout;
};

#endif

