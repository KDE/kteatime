/* -------------------------------------------------------------

   tealist.cpp

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

#include <klocale.h>
#include <qlistview.h>
#include "tealist.h"


QString int2time(int time)
{
	QString str;
	if (time / 60)
		str.append(i18n("%1 min").arg(time / 60));
	if (time % 60)
          if (str.isEmpty())
            str.append(i18n("%1 s").arg(time % 60));
          else
            str.append(i18n(" %1 s").arg(time % 60));
	return str;
}


TeaListItem::TeaListItem(QListView * parent)
    :QListViewItem(parent)
{

}

TeaListItem::TeaListItem(QListView * parent, QListViewItem *after)
    :QListViewItem(parent, after)
{

}

TeaListItem::~TeaListItem()
{
}

void TeaListItem::setTime(int t)
{
	QListViewItem::setText(1, int2time(t));
	tim = t;
}

void TeaListItem::setName(const QString &n)
{
	nam = n;
	QListViewItem::setText(0, n);
}

QString TeaListItem::name()
{
	return nam;
}

int TeaListItem::time()
{
	return tim;
}
