/* -------------------------------------------------------------

   tealist.h

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

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
	void setName(QString n);

private:
	int tim;
	QString nam;
};


QString int2time(int t);
