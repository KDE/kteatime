/* -------------------------------------------------------------

   tealist.cpp

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

#include <klocale.h>
#include <q3listview.h>
#include "tealist.h"


QString int2time(int time)
{
	QString str;
	if (time / 60)
		str.append(i18n("%1 min", time / 60));
	if (time % 60)
		str.append(i18n("%1 s", time % 60));
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

QString TeaListItem::name()
{
	return nam;
}

int TeaListItem::time()
{
	return tim;
}
