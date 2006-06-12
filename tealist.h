/* -------------------------------------------------------------

   tealist.h

   Copyright 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */
#ifndef TEALIST_H
#define TEALIST_H

#include <qstring.h>

class Q3ListView;
class Q3ListViewItem;


class TeaListItem : public Q3ListViewItem
{

public:
	TeaListItem(Q3ListView *parent);
	TeaListItem(Q3ListView *parent, Q3ListViewItem *after);
	~TeaListItem();

	int time();
	QString name();
	void setTime(int v);
	void setName(const QString &n);

private:
	int tim;
	QString nam;
};


QString int2time(int t);
#endif
