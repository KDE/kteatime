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

#include <stdlib.h>
#include <assert.h>

#include <qcheckbox.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qfile.h>
#include <qcursor.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qpixmap.h>
#include <qbitmap.h>

#include <kconfig.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <knotifyclient.h>
#include <knuminput.h>
#include <kseparator.h>
#include <kpopupmenu.h>
#include <kdialogbase.h>
#include <kaction.h>
#include <knotifydialog.h>

#include "tealist.h"
#include "timeedit.h"
#include "toplevel.h"
#include "toplevel.moc"


const int TopLevel::DEFAULT_TEA_TIME = 3*60;


TopLevel::TopLevel() : KSystemTray()
{
	setBackgroundMode(X11ParentRelative);   // what for?
	QString n, key;
	unsigned int num;

	teas.clear();

	KConfig *config = kapp->config();
	config->setGroup("Teas");

	if (config->hasKey("Number")) {
		// assuming this is a new-style config
		num = config->readNumEntry("Number", 0);
		teas.resize(num);
		QString tempstr;
		for (unsigned int index=1; index<=num; ++index) {
			key.sprintf("Tea%d Time", index);
			tempstr = config->readEntry(key, NULL);
			teas[index-1].time = tempstr.toInt();
  			key.sprintf("Tea%d Name", index);
			teas[index-1].name = config->readEntry(key, NULL);
			// FIXME: check for non-existence!
  		}
		config->setGroup("General");
	} else {
		// either old-style config or first start, so provide some sensible defaults
		// (which are the same as in old-style kteatime)
		tea_struct temp;
		temp.name = i18n("Black Tea");
		temp.time = 180;
		teas.append(temp);
		temp.name = i18n("Earl Grey");
		temp.time = 300;
		teas.append(temp);
		temp.name = i18n("Fruit Tea");
		temp.time = 480;
		teas.append(temp);

		// switch back to old-style default group
		config->setGroup(NULL);
		// look for old-style "UserTea"-entry and add that one also
		if (config->hasKey("UserTea")) {
			num = config->readNumEntry("UserTea", 150);
			temp.name = i18n("Other Tea");
			temp.time = num;
			teas.append(temp);
		}
	}
	current_selected = config->readNumEntry("Tea", 0);
	if (current_selected >= teas.count())
		current_selected = 0;

	listempty = (teas.count() == 0);


	startAct = new KAction(i18n("&Start"), "1rightarrow", 0,
	                       this, SLOT(start()), actionCollection(), "start");
	stopAct = new KAction(i18n("Sto&p"), "cancel", 0,
	                      this, SLOT(stop()), actionCollection(), "stop");
	confAct = new KAction(i18n("&Configure..."), "configure", 0,
	                      this, SLOT(config()), actionCollection(), "configure");
	anonAct = new KAction(i18n("&Anonymous..."), 0, 0,
	                      this, SLOT(anonymous()), actionCollection(), "anonymous");
//	KAction *quitAct = actionCollection()->action("file_quit");

	// create app menu (displayed on right-click)
	menu = new QPopupMenu();
	menu->setCheckable(true);
	connect(menu, SIGNAL(activated(int)), this, SLOT(teaSelected(int)));

	// this menu will be displayed when no tea is steeping, and left mouse button is clicked
	start_menu = new QPopupMenu();
	start_menu->setCheckable(true);     // menu isn't tickable, but this gives some add. spacing
	connect(start_menu, SIGNAL(activated(int)), this, SLOT(teaStartSelected(int)));

	rebuildTeaMenus();      // populate tops of menus with tea-entries from config

	KHelpMenu* help = new KHelpMenu(this, KGlobal::instance()->aboutData(), false);
	KPopupMenu* helpMnu = help->menu();

	start_menu->insertSeparator();
	anonAct->plug(start_menu);

	menu->insertSeparator();
	anonAct->plug(menu);
	startAct->plug(menu);
	stopAct->plug(menu);
	menu->insertSeparator();
	confAct->plug(menu);
	menu->insertItem(SmallIcon("help"), i18n("&Help"), helpMnu);
	menu->insertItem(SmallIcon("exit"), i18n("Quit"), kapp, SLOT(quit()));
//	quitAct->plug(menu);    // FIXME: this doesn't seem to work with above definition of quitAct?
	                        //        (need special 'quit'-method?)

	// this menu will be displayed when a tea is steeping, and left mouse button is clicked
	steeping_menu = new QPopupMenu();
//	steeping_menu->insertItem(SmallIcon("cancel"), i18n("Just &Cancel Current"), this, SLOT(stop()));
	stopAct->plug(steeping_menu);   // FIXME: can provide different text for this incarnation?

//	start_menu->insertSeparator();
//	startAct->plug(start_menu);     // FIXME: include "start" entry here for quick access to current tea?

	// read remaining entries from config-file
	useNotify = config->readBoolEntry("Beep", true);    // "Beep" should really be named "Notify"
	usePopup = config->readBoolEntry("Popup", true);
	useAction = config->readBoolEntry("UseAction", true);
	action = config->readEntry("Action");
	useTrayVis = config->readBoolEntry("UseTrayVis", true);

	mugPixmap = loadIcon("mug");
	teaNotReadyPixmap = loadIcon("tea_not_ready");
	teaAnim1Pixmap = loadIcon("tea_anim1");
	teaAnim2Pixmap = loadIcon("tea_anim2");

	confdlg = 0L;
	anondlg = 0L;

	stop();                         // reset timer, disable some menu entries, etc.
}

/* slot: signal shutDown() from KApplication */
/* (not currently needed)
void TopLevel::queryExit()
{
	KConfig *config = kapp->config();
//	config->sync();
}
*/


/** Destructor */
TopLevel::~TopLevel()
{
	delete menu;
	delete steeping_menu;
	delete start_menu;
	// FIXME: must delete more (like all the QWidgets in config-window)?
}


/** Handle mousePressEvent */
void TopLevel::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == LeftButton) {
		if (ready) {
			stop();                         // reset tooltip and stop animation
		} else {
			if (running)
				steeping_menu->popup(QCursor::pos());
			else
				start_menu->popup(QCursor::pos());
		}
	} else if (event->button() == RightButton)
		menu->popup(QCursor::pos());
//	else if (event->button() == MidButton)  // currently unused
}

/** Handle paintEvent (ie. animate icon) */
void TopLevel::paintEvent(QPaintEvent *)
{
	QPixmap *pm = &mugPixmap;

	if (running) {
		if (useTrayVis)
			pm = &teaAnim1Pixmap;                            // this is 'mugPixmap' plus brown content
		else
			pm = &teaNotReadyPixmap;                         // generic "steeping" icon
	} else {
		// use simple two-frame "animation"
		// FIXME: how about using a QMovie instead? (eg. MNG)
		if (ready) {
			if (firstFrame)
				pm = &teaAnim1Pixmap;
			else
				pm = &teaAnim2Pixmap;
		}
	}

	// overlay pie chart onto tray icon
	QPixmap base(*pm);                                      // make copy of base pixmap
	if (useTrayVis && running) {
		// extend mask
		QBitmap mask = *(base.mask());
		QPainter pm(&mask);
		pm.setBrush(Qt::color1);                            // fill with "foreground-colour"
		pm.setPen(Qt::NoPen);                               // no border needed/wanted
		pm.drawPie(0+1, 9+1, 11, 11, 90*16, -360*16);       // full circle of small size
		pm.drawPie(0, 9, 13, 13, 90*16, percentDone*16);    // pie part of big size
		pm.end();
		base.setMask(mask);

		// draw pie chart
		QPainter px(&base);
		px.setPen(QPen(Qt::black, 0));                      // black border
		px.setBrush(QColor(192, 0, 0));                     // red fill colour for small circle
		px.drawPie(0+1, 9+1, 11, 11, 90*16, -360*16);

		px.setBrush(QColor(0, 192, 0));                     // green fill colour for pie part
		px.drawPie(0, 9, 13, 13, 90*16, percentDone*16);
		px.end();
	}
	// FIXME: over-emphasize first and last few percent? (for better visibility)
	// FIXME: some optimizations (eg. store pre-drawn QPixmap with small circle)
	//        (and use drawEllipse() instead of drawPie() for small circle!)

	// set new tray icon
	QPainter p(this);
	int x = 1 + (12 - pm->width()/2);
	int y = 1 + (12 - pm->height()/2);
	p.drawPixmap(x, y, base);
	p.end();
}

/** Check timer and initiate appropriate action if finished */
void TopLevel::timerEvent(QTimerEvent *)
{
	if (running) {
		// a tea is steeping; must count down
		seconds--;

		if (seconds <= 0) {
			// timer has run out; notify user
			running = false;
			ready = true;
			enable_menuEntries();
			if (shooting) {
				// re-check current tea
				shooting = false;
				if (!listempty)
					menu->setItemChecked(current_selected, true);
			}

			QString teaMessage = i18n("The %1 is now ready!").arg(current_name);
			// invoke action
			if (useNotify) {
				KNotifyClient::event(winId(), "tea", teaMessage);
			}
			if (useAction && (!action.isEmpty())) {
				QString cmd = action;
				cmd.replace("%t", current_name);
				system(QFile::encodeName(cmd));
			}
			if (usePopup)
				KPassivePopup::message(i18n("The Tea Cooker"),
				                       teaMessage, teaAnim1Pixmap, this, "popup", 0);
				// FIXME: does auto-deletion work without timeout?
			setToolTip(teaMessage);
			repaint();
		} else {
			// timer not yet run out; just update tray-icon (if configured)...
			if (useTrayVis) {
				int pDone = (360 * (startSeconds - seconds)) / startSeconds;
				if (pDone - percentDone > 8) {
					// update icon not every second, but only if somewhat noticable
					percentDone = pDone;
					repaint();
				}
			}
			// ...and Tooltip
			QString min = int2time(seconds);
			setToolTip(i18n("%1 left for %2").arg(min).arg(current_name));
		}
	} else {
		// no tea is steeping; just animate icon
		if (ready) {
			firstFrame = !firstFrame;
			repaint();
		}
	}
}

/** update ToolTip */
void TopLevel::setToolTip(const QString &text, bool force)
{
	// don't update if text hasn't changed
	if (lastTip == text)
		return;

	// don't remove Tooltip if (probably - can't know for sure?) currently showing
	// FIXME: this isn't too nice: currently mouse must stay outside for >1s for update to occur
	if (force || !this->hasMouse() || (lastTip == i18n("The Tea Cooker"))) {
		lastTip = text;
		QToolTip::remove(this);
		QToolTip::add(this, text);
	}
}


/** add all configured teas to both menus */
void TopLevel::rebuildTeaMenus() {
	// first remove all current tea-entries from menus; these can be identified by their positive id
	while (menu->idAt(0) >= 0)
		menu->removeItemAt(0);          // remove from right-click menu
	while (start_menu->idAt(0) >= 0)
		start_menu->removeItemAt(0);    // remove from left-click menu

	// now add new tea-entries to top of menus
	int id = 0;
	int index = 0;
	for (QValueVector<tea_struct>::ConstIterator it=teas.begin(); it != teas.end(); ++it) {
		// construct string with name and steeping time
		QString str = it->name;
		str.append(" (");
		str.append(int2time(it->time));
		str.append(")");

		start_menu->insertItem(str, id, index);     // add to left-click menu
		menu->insertItem(str, id++, index++);       // add to right-click menu
	}

	// now select 'current' tea
	if (!listempty)
		menu->setItemChecked(current_selected, true);   // all others aren't checked,
		                                                // because we just added them
}

/* enable/disable menu-entries according to current running-state */
void TopLevel::enable_menuEntries()
{
	for (int index=0; menu->idAt(index) >= 0; ++index) {
		// [en|dis]able all tea-entries (all have positive menu-ids)
		menu->setItemEnabled(menu->idAt(index), !running);
	}

	startAct->setEnabled(!running);     // "start" entry
	stopAct->setEnabled(running);       // "stop" entry
	confAct->setEnabled(!running);      // "configuration" entry
	anonAct->setEnabled(!running);      // "anonymous" entry
}

/* menu-slot: tea selected in tea-menu */
void TopLevel::teaSelected(int index)
{
	if (index >=0 && (unsigned int)index < teas.count()) {
		// tick new active item in menu
		menu->setItemChecked(current_selected, false);
		menu->setItemChecked(index, true);

		current_selected = index;
		KConfig *config = kapp->config();
		config->setGroup("General");
		config->writeEntry("Tea", current_selected);
	}
	// all other entries of this menu have custom handlers
}

/* start_menu-slot: tea selected (and activated!) in tea-menu */
void TopLevel::teaStartSelected(int index)
{
	if (index >=0 && (unsigned int)index < teas.count()) {
		teaSelected(index);

		start();
	}
}

/* menu-slot: "start" selected in menu */
void TopLevel::start()
{
	if (listempty && !shooting) {
		KMessageBox::error(this, i18n("There is no tea to begin steeping."), i18n("No Tea"));
	} else {
		if (!shooting) {
			current_name = teas[current_selected].name;     // remember name of current tea
			startSeconds = teas[current_selected].time;     // initialize time for current tea
			seconds = startSeconds;
			percentDone = 0;
		}
		// else both are already defined by dialog handler

		killTimers();
		startTimer(1000);                               // 1000ms = 1s (sufficient resolution)

		running = true;
		ready = false;
		enable_menuEntries();                           // disable "start", enable "stop"

		repaint();
	}
}

/* menu-slot: "stop" selected in menu */
void TopLevel::stop()
{
	killTimers();

	running = false;
	ready = false;
	enable_menuEntries();                               // disable "top", enable "start"
	if (shooting) {
		// re-check current tea
		shooting = false;
		if (!listempty)
			menu->setItemChecked(current_selected, true);
	}

	setToolTip(i18n("The Tea Cooker"), true);
	repaint();
}

/* open dialog to start an 'anonymous' tea */
void TopLevel::anonymous()
{
	if (!anondlg) {
		// FIXME: dialog appears centered on screen, but should be near systray icon!
		anondlg = new KDialogBase(KDialogBase::Plain, i18n("Anonymous Tea"),
		                          KDialogBase::Ok | KDialogBase::Cancel,
		                          KDialogBase::Ok, this, "anonymous", true);
		QWidget *page = anondlg->plainPage();
		QBoxLayout *top_box = new QVBoxLayout(page);
		QBoxLayout *prop_box = new QHBoxLayout(top_box);
		QVBox *propleft = new QVBox(page);
		prop_box->addWidget(propleft);
		QVBox *propright = new QVBox(page);
		prop_box->addWidget(propright);

		anon_time = new TimeEdit(propright);
		anon_time->setFixedHeight(anon_time->sizeHint().height());
		anon_time->setValue(DEFAULT_TEA_TIME);
		QLabel *l = new QLabel(anon_time, i18n("Tea time:"), propleft);
		l->setFixedSize(l->sizeHint());

		top_box->addStretch();

		anon_time->setFocus();
	} else {
		// FIXME: do what here?
		// reset time to DEFAULT_TEA_TIME?
		// (why? - better use LRU, and save that to config)
	}

	if (anondlg->exec() == QDialog::Accepted) {
		shooting = true;
		if (!listempty)
			menu->setItemChecked(current_selected, false);  // no item is to be checked
		current_name = i18n("tea");                         // some generic tea name
		startSeconds = anon_time->value();
		seconds = startSeconds;
		percentDone = 0;
		start();
	}
}




//
// Configure-window handling
//


/* enable/disable buttons for editing listbox */
void TopLevel::enable_controls() {
	bool haveSelection = (listbox->currentItem() != 0);
	bool amFirst = (listbox->currentItem() == listbox->firstChild());
	bool amLast = true;
	if (haveSelection)
		amLast = (!listbox->currentItem()->itemBelow());   // itemBelow() returns returns NULL if last

	btn_del->setEnabled(haveSelection);
	btn_up->setEnabled(haveSelection && !amFirst);
	btn_down->setEnabled(haveSelection && !amLast);
	if (haveSelection)
		listbox->ensureItemVisible(listbox->currentItem());
}

/* disable right side of configure-window */
void TopLevel::disable_properties() {
	editgroup->setEnabled(false);
}

/* enable right side of configure-window */
void TopLevel::enable_properties() {
	editgroup->setEnabled(true);
}

/* config-slot: item in tea-list selected */
void TopLevel::listBoxItemSelected() {
	if (listbox->currentItem()) {
		// item selected, display its properties on right side
		nameEdit->setText(static_cast<TeaListItem *>(listbox->currentItem())->name());
		timeEdit->setValue(static_cast<TeaListItem *>(listbox->currentItem())->time());
		enable_controls();
	}
}

/* config-slot: name of a tea edited */
void TopLevel::nameEditTextChanged(const QString& newText) {
	/* this method also gets called when the last TeaListItem has been deleted
	 * (to clear the name edit widget), so check for empty listbox */
	if (listbox->currentItem() != NULL) {
		listbox->blockSignals(TRUE);
		static_cast<TeaListItem *>(listbox->currentItem())->setName(newText);
		listbox->blockSignals(FALSE);
	}
}

/* config-slot: time for a tea changed */
void TopLevel::spinBoxValueChanged(int v) {
	/* this method also gets called when the last TeaListItem has been deleted
	 * (to clear the time edit widget), so check for empty listbox */
	if (listbox->currentItem() != NULL)
		static_cast<TeaListItem *>(listbox->currentItem())->setTime(v);
}

/* config-slot: "new" button clicked */
void TopLevel::newButtonClicked() {
	TeaListItem* item = new TeaListItem(listbox, listbox->currentItem());
	listbox->setCurrentItem(item);

	nameEdit->setText(i18n("New Tea"));
	timeEdit->setValue(DEFAULT_TEA_TIME);

	nameEdit->setFocus();

	if (listbox->childCount() == 1) {
		enable_properties();
		current_item = item;
	}
	enable_controls();
}

/* config-slot: "delete" button clicked */
void TopLevel::delButtonClicked() {
	if (listbox->currentItem()) {
		TeaListItem *curritem = static_cast<TeaListItem *>(listbox->currentItem());

		if (listbox->childCount() == 1) {
			// no childs left after we've deleted this item
			listbox->setSelected(listbox->currentItem(), false);
			nameEdit->setText("");
			timeEdit->setValue(0);
			disable_properties();
		} else {
			// select new current item
			if (listbox->firstChild() != curritem)
				listbox->setSelected(listbox->firstChild(), true);
			else
				listbox->setSelected(listbox->firstChild()->nextSibling(), true);
		}

		delete curritem;
		enable_controls();
	}
}

/* config-slot: "up" button clicked */
void TopLevel::upButtonClicked() {
	QListViewItem* item = listbox->currentItem();

	if (item && item->itemAbove())
		item->itemAbove()->moveItem(item);

	enable_controls();
}

/* config-slot: "down" button clicked */
void TopLevel::downButtonClicked() {
	QListViewItem* item = listbox->currentItem();

	if (item && item->itemBelow())
		item->moveItem(item->itemBelow());

	enable_controls();
}

/* config-slot: checkbox next to "action" field toggled*/
void TopLevel::actionEnableToggled(bool on)
{
	actionEdit->setEnabled(on);
}

/* config-slot: "help" button clicked */
void TopLevel::help()
{
	kapp->invokeHelp();
}

/* config-slot: "Configure Events..." button clicked */
void TopLevel::confButtonClicked()
{
	KNotifyDialog::configure(btn_conf);
}


void TopLevel::config()
{
  if (!confdlg) {
    confdlg = new KDialogBase(KDialogBase::Plain, i18n("Configure Tea Cooker"),
                              KDialogBase::Ok|KDialogBase::Cancel|KDialogBase::Help,
                              KDialogBase::Ok, this, "config", true);
    QWidget *page = confdlg->plainPage();
    // FIXME: enforce sensible initial/default size of dialog
    // FIXME: modal is ok, but can avoid always-on-top?

    QBoxLayout *top_box = new QVBoxLayout(page, 0, 8);    // whole config-stuff
    QBoxLayout *box = new QHBoxLayout(top_box);           // list + properties

    /* left side - tea list and list-modifying buttons */
    QBoxLayout *leftside = new QVBoxLayout(box);
    QGroupBox *listgroup = new QGroupBox(2, Vertical, i18n("Tea List"), page);
    leftside->addWidget(listgroup, 0, 0);

    listbox = new QListView(listgroup, "listBox");
    listbox->addColumn(i18n("Name"));
    listbox->header()->setClickEnabled(false, listbox->header()->count()-1);
    listbox->addColumn(i18n("Time"));
    listbox->header()->setClickEnabled(false, listbox->header()->count()-1);
    listbox->setSorting(-1);
    connect(listbox, SIGNAL(selectionChanged()), SLOT(listBoxItemSelected()));

    // now buttons for editing the tea-list
    QWidget *listgroup_widget = new QWidget(listgroup);
    QBoxLayout *hbox = new QHBoxLayout(listgroup_widget);
    hbox->setSpacing(4);
    btn_new = new QPushButton(QString::null, listgroup_widget);
    QToolTip::add(btn_new, i18n("New"));
    btn_new->setPixmap(SmallIcon("filenew"));
    btn_new->setMinimumSize(btn_new->sizeHint() * 1.2);
    connect(btn_new, SIGNAL(clicked()), SLOT(newButtonClicked()));
    hbox->addWidget(btn_new);

    btn_del = new QPushButton(QString::null, listgroup_widget);
    QToolTip::add(btn_del, i18n("Delete"));
    btn_del->setIconSet(SmallIconSet("editdelete"));
    btn_del->setMinimumSize(btn_new->sizeHint() * 1.2);
    connect(btn_del, SIGNAL(clicked()), SLOT(delButtonClicked()));
    hbox->addWidget(btn_del);

    btn_up = new QPushButton(QString::null, listgroup_widget);
    QToolTip::add(btn_up, i18n("Up"));
    btn_up->setIconSet(SmallIconSet("up"));
    btn_up->setMinimumSize(btn_up->sizeHint() * 1.2);
    connect(btn_up, SIGNAL(clicked()), SLOT(upButtonClicked()));
    hbox->addWidget(btn_up);

    btn_down = new QPushButton(QString::null, listgroup_widget);
    QToolTip::add(btn_down, i18n("Down"));
    btn_down->setIconSet(SmallIconSet("down"));
    btn_down->setMinimumSize(btn_down->sizeHint() * 1.2);
    connect(btn_down, SIGNAL(clicked()), SLOT(downButtonClicked()));
    hbox->addWidget(btn_down);

    hbox->addStretch(10);

    /* right side - tea properties */
    QBoxLayout *rightside = new QVBoxLayout(box);
    editgroup = new QGroupBox(2, Vertical, i18n("Tea Properties"), page);
    rightside->addWidget(editgroup, 0, 0);
    QHBox *propbox = new QHBox(editgroup);

    // FIXME: - must enforce correct vertical alignment of each label-editor pair
    //          (better use one HBox for each label-editor pair?)
    QVBox *propleft = new QVBox(propbox);
    QVBox *propright = new QVBox(propbox);
    nameEdit = new QLineEdit(propright);
    nameEdit->setFixedHeight(nameEdit->sizeHint().height());
    nameEdit->setAlignment(QLineEdit::AlignLeft);
    QLabel *l = new QLabel(nameEdit, i18n("Name:"), propleft);
    l->setFixedSize(l->sizeHint());
    connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(nameEditTextChanged(const QString&)) );

    timeEdit = new TimeEdit(propright);
    timeEdit->setFixedHeight(timeEdit->sizeHint().height());
    l = new QLabel(timeEdit, i18n("Tea time:"), propleft);
    l->setFixedSize(l->sizeHint());
    connect(timeEdit, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)));

    /* bottom - timeout actions */
    QGroupBox *actiongroup = new QGroupBox(4, Vertical, i18n("Action"), page);
    top_box->addWidget(actiongroup, 0, 0);

    QWidget *actionconf_widget = new QWidget(actiongroup);
    QBoxLayout *actionconf_hbox = new QHBoxLayout(actionconf_widget);
    btn_conf = new QPushButton(i18n("Configure Events..."), actionconf_widget);
    actionconf_hbox->addWidget(btn_conf);
    connect(btn_conf, SIGNAL(clicked()), SLOT(confButtonClicked()));
    actionconf_hbox->addStretch(10);

    eventEnable = new QCheckBox(i18n("Event"), actiongroup);
    popupEnable = new QCheckBox(i18n("Popup"), actiongroup);
    eventEnable->setFixedHeight(eventEnable->sizeHint().height());
    popupEnable->setFixedHeight(popupEnable->sizeHint().height());

    QHBox *actionbox = new QHBox(actiongroup);
    actionEnable = new QCheckBox(actionbox);
//    FIXME: add text to this line:
//    QLabel *actionLabel = new QLabel(i18n("Execute: "), actiongroup);
    actionEdit = new QLineEdit(actionbox);
    actionEdit->setFixedHeight(actionEdit->sizeHint().height());
    QToolTip::add(actionEdit, i18n("Enter command here; '%t' will be replaced with name of steeping tea"));
    connect(actionEnable, SIGNAL(toggled(bool)), SLOT(actionEnableToggled(bool)));
    rightside->addStretch();

    // single checkbox
    visEnable = new QCheckBox(i18n("Visualize progress in icon tray"), page);
    top_box->addWidget(visEnable, 0, 0);


    // let listbox claim all remaining vertical space
    top_box->setStretchFactor(box, 10);

    connect(confdlg, SIGNAL(helpClicked()), SLOT(help()));
  }

  // now add all defined teas (and their times) to the listview
  // this is done backwards because QListViewItems are inserted at the top
  listbox->clear();
  for (int i=teas.count()-1; i>=0; i--) {
    TeaListItem *item = new TeaListItem(listbox);
    item->setName(teas[i].name);
    item->setTime(teas[i].time);
    if ((unsigned int)i == current_selected)
      current_item = item;
  }

  // select first entry in listbox; if no entries present then disable right side
  if (listempty) {
    enable_controls();
    disable_properties();
    nameEdit->setText("");
    timeEdit->setValue(1);
  } else {
    listbox->setSelected(listbox->firstChild(), true);
  }

  // -------------------------

  eventEnable->setChecked(useNotify);
  popupEnable->setChecked(usePopup);
  actionEnable->setChecked(useAction);
  actionEdit->setText(action);
  actionEdit->setEnabled(useAction);
  visEnable->setChecked(useTrayVis);

  if (confdlg->exec() == QDialog::Accepted)
  {
    // activate new settings
    useNotify = eventEnable->isChecked();
    usePopup = popupEnable->isChecked();
    useAction = actionEnable->isChecked();
    action = actionEdit->text();
    useTrayVis = visEnable->isChecked();

    teas.clear();

    // Copy over teas and times from the QListView
    int i = 0;
    teas.clear();
    teas.resize(listbox->childCount());
    for (QListViewItemIterator it(listbox); it.current() != 0; ++it) {
      teas[i].name = static_cast<TeaListItem *>(it.current())->name();
      teas[i].time = static_cast<TeaListItem *>(it.current())->time();
      if (it.current() == current_item)
        current_selected = i;
      i++;
    }

    listempty = (teas.count() == 0);

    rebuildTeaMenus();

    // and store to config
    KConfig *config = kapp->config();
    // remove old-style entries from default-group (if present)
    if (config->hasKey("UserTea"))
      config->deleteEntry("UserTea");

    config->setGroup("General");
    config->writeEntry("Beep", useNotify);
    config->writeEntry("Popup", usePopup);
    config->writeEntry("UseAction", useAction);
    config->writeEntry("Action", action);
    config->writeEntry("Tea", current_selected);
    config->writeEntry("UseTrayVis", useTrayVis);
    // first get rid of all previous tea-entries from config, then write anew
    config->deleteGroup("Teas", true);          // deep remove of whole group
    config->setGroup("Teas");
    config->writeEntry("Number", teas.count());
    QString key;
    int index = 1;
    for (QValueVector<tea_struct>::ConstIterator it = teas.begin(); it != teas.end(); ++it) {
      key.sprintf("Tea%d Name", index);
      config->writeEntry(key, it->name);
      key.sprintf("Tea%d Time", index);
      config->writeEntry(key, it->time);
      index++;
    }

    config->sync();
  }
}
