/* -------------------------------------------------------------

   toplevel.h

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kapplication.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qvaluevector.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <knuminput.h>
#include <ksystemtray.h>

class QPixmap;
class KAction;
class TimeEdit;

class TopLevel : public KSystemTray
{
	Q_OBJECT

public:

	TopLevel();
	~TopLevel();

protected:

	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);
	void timerEvent(QTimerEvent *);

private slots:

	void teaSelected(int index);
	void teaStartSelected(int index);
	void start();
	void stop();
	void config();
	void help();
	void anonymous();
	void setToolTip(const QString &text);
	void rebuildTeaMenus();

	void listBoxItemSelected();
	void nameEditTextChanged(const QString& newText);
	void spinBoxValueChanged(int v);
	void newButtonClicked();
	void delButtonClicked();
	void upButtonClicked();
	void downButtonClicked();
	void confButtonClicked();
	void enable_menuEntries();
	void disable_properties();
	void enable_properties();
	void enable_controls();
	void actionEnableToggled(bool on);

private:

	struct tea_struct {
		QString name;
		int time;
	};
	QValueVector<tea_struct> teas;      // list of tea-names and times

	bool running, ready, firstFrame, listempty;
	int seconds;                        // variable for counting down seconds

	unsigned current_selected;          // index of currently +selected+ tea in menu
	QListViewItem *current_item;        // ptr to currently +selected+ tea in ListView
	QString current_name;               // name of currently +running+ tea (if any)
	bool shooting;                      // anonymous tea currently steeping?

	bool useNotify, usePopup, useAction;
	QString action;

	QPixmap *mugPixmap, *teaNotReadyPixmap, *teaAnim1Pixmap, *teaAnim2Pixmap;

	KAction *startAct, *stopAct, *confAct, *anonAct;
	QPopupMenu *menu, *steeping_menu, *start_menu;
	QListView *listbox;
	QLineEdit *nameEdit, *actionEdit;
	TimeEdit *timeEdit;
	QGroupBox *editgroup;
	QPushButton *btn_new, *btn_del, *btn_up, *btn_down, *btn_conf;

	QString lastTip;
};

#endif
