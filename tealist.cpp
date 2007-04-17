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

#include "tealist.h"

#include <klocale.h>
#include <q3listview.h>

QString int2time(int time)
{
	QString str;
	if (time / 60)
		str.append(i18n("%1 min", time / 60));
	if (time % 60)
          if (str.isEmpty())
            str.append(i18n("%1 s", time % 60));
          else
            str.append(i18n(" %1 s", time % 60));
	return str;
}


TeaListItem::TeaListItem(Q3ListView * parent)
    :Q3ListViewItem(parent)
{

}

TeaListItem::TeaListItem(Q3ListView * parent, Q3ListViewItem *after)
    :Q3ListViewItem(parent, after)
{

}

TeaListItem::~TeaListItem()
{
}

void TeaListItem::setTime(int t)
{
	Q3ListViewItem::setText(1, int2time(t));
	tim = t;
}

void TeaListItem::setName(const QString &n)
{
	nam = n;
	Q3ListViewItem::setText(0, n);
}

QString TeaListItem::name() const
{
	return nam;
}

int TeaListItem::time() const
{
	return tim;
}
