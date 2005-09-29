/*
 *   This file is part of the KTeaTime application.
 *
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel (hoelzer@kde.org)
 *   Copyright (C) 2002-2003  Martin Willers (willers@xm-arts.de)
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

#ifndef TOPLEVEL_H
#define TOPLEVEL_H

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
#include <qpixmap.h>

class KAction;
class KDialogBase;
class QCheckBox;
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
	void setToolTip(const QString &text, bool force=false);
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

	static const int DEFAULT_TEA_TIME;

	struct tea_struct {
		QString name;
		int time;
	};
	QValueVector<tea_struct> teas;      // list of tea-names and times

	bool running, ready, firstFrame, listempty;
	int seconds;                        // variable for counting down seconds
	int startSeconds;                   // steeping time for current tea
	int percentDone;                    // ok, this isn't really "per 100", but "per 360"

	unsigned current_selected;          // index of currently +selected+ tea in menu
	QListViewItem *current_item;        // ptr to currently +selected+ tea in ListView
	QString current_name;               // name of currently +running+ tea (if any)
	bool shooting;                      // anonymous tea currently steeping?

	bool useNotify, usePopup, useAction;
	QString action;
	bool useTrayVis;                    // visualize progress in tray icon

	QPixmap mugPixmap, teaNotReadyPixmap, teaAnim1Pixmap, teaAnim2Pixmap;

	KAction *startAct, *stopAct, *confAct, *anonAct;
	QPopupMenu *menu, *steeping_menu, *start_menu;
	QListView *listbox;
	QLineEdit *nameEdit, *actionEdit;
	TimeEdit *timeEdit;
	QGroupBox *editgroup;
	QPushButton *btn_new, *btn_del, *btn_up, *btn_down, *btn_conf;

	QString lastTip;
	KDialogBase *anondlg, *confdlg;
	TimeEdit *anon_time;
	QCheckBox *eventEnable, *popupEnable, *actionEnable, *visEnable;
};

#endif
