/* -------------------------------------------------------------

   tealist.h

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */
#ifndef TEALIST_H
#define TEALIST_H

#include <qstring.h>

class QListView;
class QListViewItem;


class TeaListItem : public QListViewItem
{

public:
	TeaListItem(QListView *parent);
	TeaListItem(QListView *parent, QListViewItem *after);
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
