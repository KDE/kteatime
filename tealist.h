/* -------------------------------------------------------------

   tealist.h

   Copyright 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */
#ifndef TEALIST_H
#define TEALIST_H

#include <qstring.h>
#include <Q3ListViewItem>
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
