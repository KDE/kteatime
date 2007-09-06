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

#ifndef TEALIST_H
#define TEALIST_H

#include <QtCore/QString>
#include <Qt3Support/Q3ListViewItem>

class Q3ListView;

class TeaListItem : public Q3ListViewItem
{

public:
	explicit TeaListItem(Q3ListView *parent);
	TeaListItem(Q3ListView *parent, Q3ListViewItem *after);
	~TeaListItem();

	int time() const;
	QString name() const;
	void setTime(int v);
	void setName(const QString &n);

private:
	int tim;
	QString nam;
};


QString int2time(int t);
#endif
