/* -------------------------------------------------------------

   toplevel.cpp

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

   Some additions by Martin Willers <willers@xm-arts.de>, 2002,2003

   With contributions from Daniel Teske <teske@bigfoot.com>, and
   Jackson Dunstan <jdunstan@digipen.edu>

 ------------------------------------------------------------- */

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

#include "toplevel.h"
#include "toplevel.moc"


TopLevel::TopLevel() : KSystemTray()
{
	setBackgroundMode(X11ParentRelative);   // what for?
	QString n, key;
	unsigned int num;

	teas.clear();
	times.clear();

	KConfig *config = kapp->config();
	config->setGroup("Teas");

	if (config->hasKey("Number")) {
		// assuming this is a new-style config
		num = config->readNumEntry("Number", 0);
		times.resize(num);
		teas.resize(num);
		for (unsigned int index=1; index<=num; index++) {
			key.sprintf("Tea%d Time", index);
			times[index-1] = config->readEntry(key, NULL);
  			key.sprintf("Tea%d Name", index);
			teas[index-1] = config->readEntry(key, NULL);
			// FIXME: check for non-existence!
  		}
		config->setGroup("General");
	} else {
		// either old-style config or first start, so provide some sensible defaults
		// (which are the same as in old-style kteatime)
		teas.append(i18n("Black Tea")); n.setNum(180); times.append(n);
		teas.append(i18n("Earl Grey")); n.setNum(300); times.append(n);
		teas.append(i18n("Fruit Tea")); n.setNum(480); times.append(n);
		// switch back to old-style default group
		config->setGroup(NULL);
		// look for old-style "UserTea"-entry and add that one also
		if (config->hasKey("UserTea")) {
			num = config->readNumEntry("UserTea", 150);
			n = i18n("Other Tea");
			teas.append(n); n.setNum(num); times.append(n);
		}
	}
	current_selected = config->readNumEntry("Tea", 0);
	// sanity-check for this is done on rebuildTeaMenus()


	startAct = new KAction("&Start", "1rightarrow", 0,
	                       this, SLOT(start()), actionCollection(), "start");
	stopAct = new KAction("Sto&p", "cancel", 0,
	                      this, SLOT(stop()), actionCollection(), "stop");
	confAct = new KAction("&Configure...", "configure", 0,
	                      this, SLOT(config()), actionCollection(), "configure");
//	KAction *quitAct = actionCollection()->action("file_quit");

	// create app menu (displayed on right-click)
	menu = new QPopupMenu();
	menu->setCheckable(true);
	connect(menu, SIGNAL(activated(int)), this, SLOT(teaSelected(int)));

	// this menu will be displayed when no tea is steeping, and left mouse button is clicked
	start_menu = new QPopupMenu();
	start_menu->setCheckable(true);     // menu isn't tickable, but this gives some add. spacing
	connect(start_menu, SIGNAL(activated(int)), this, SLOT(teaStartSelected(int)));

	rebuildTeaMenus();		// populate tops of menus with tea-entries from config

	KHelpMenu* help = new KHelpMenu(this, KGlobal::instance()->aboutData(), false);
	KPopupMenu* helpMnu = help->menu();

	menu->insertSeparator();
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
//	steeping_menu->insertItem(SmallIcon("cancel"), i18n("Just &cancel current"), this, SLOT(stop()));
	stopAct->plug(steeping_menu);   // FIXME: can provide different text for this incarnation?

//	start_menu->insertSeparator();
//	startAct->plug(start_menu);     // FIXME: include "start" entry here for quick access to current tea?

	// read remaining entries from config-file
	beeping = config->readBoolEntry("Beep", true);
	popping = config->readBoolEntry("Popup", true);
	useAction = config->readBoolEntry("UseAction", true);
	action = config->readEntry("Action");

	mugPixmap = new QPixmap(UserIcon("mug"));
	teaNotReadyPixmap = new QPixmap(UserIcon("tea_not_ready"));
	teaAnim1Pixmap = new QPixmap(UserIcon("tea_anim1"));
	teaAnim2Pixmap = new QPixmap(UserIcon("tea_anim2"));

	stop();				// reset timer, disable some menu entries, etc.
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
	delete mugPixmap;
	delete teaNotReadyPixmap;
	delete teaAnim1Pixmap;
	delete teaAnim2Pixmap;
	delete steeping_menu;
	delete start_menu;
	// FIXME: must delete more (like all the QWidgets in config-window)?
	//        (or the QPixmaps?)
}


/** Handle mousePressEvent */
void TopLevel::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == LeftButton) {
		if (ready) {
			stop();			// reset tooltip and stop animation
		} else {
			if (running)
				steeping_menu->popup(QCursor::pos());
			else
				start_menu->popup(QCursor::pos());
		}
	} else if (event->button() == RightButton)
		menu->popup(QCursor::pos());
//	else if (event->button() == MidButton)	// currently unused
}


/** Handle paintEvent (ie. animate icon if tea is finished) */
void TopLevel::paintEvent(QPaintEvent *)
{
	QPixmap *pm;

	if (running)
		pm = teaNotReadyPixmap;
	else {
		if (ready) {
			if (firstFrame)
				pm = teaAnim1Pixmap;
			else
				pm = teaAnim2Pixmap;
		} else
			pm = mugPixmap;
	}

	QPainter p(this);
	int x = 1 + (12 - pm->width()/2);
	int y = 1 + (12 - pm->height()/2);
	p.drawPixmap(x , y, *pm);
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

			QString teaMessage = i18n("The %1 is now ready!").arg(current_name);
			// invoke action
			if (beeping) {
//				KNotifyClient::beep();          // this doesn't seem to work?!
				KNotifyClient::event("tea");

				// maybe use this instead (do I really need my own 'eventsrc' file?)
//				KNotifyClient::userEvent("Tea is ready",
//				                         KNotifyClient::Sound,
//				                         KNotifyClient::Notification,
//				                         "kopete_event.wav");
			}
			if (useAction && (!action.isEmpty())) {
        QString cmd = action;
        cmd.replace("%t", current_name);
				system(QFile::encodeName(cmd));
      }
			if (popping)
				KPassivePopup::message(i18n("The Tea Cooker"),
				                       teaMessage, *teaAnim1Pixmap, this, "popup", 0);
				// FIXME: does auto-deletion work without timeout?
			setToolTip(teaMessage);
			repaint();
		} else {
			// timer not yet run out; just update Tooltip appropriately
			QString min;
			// use real time-string "mm:ss" (according to locale!) for displaying remaining time
			int h = (seconds / 3600);
			int m = (seconds - h*3600) / 60;
			int s = (seconds - h*3600 - m*60);
			QTime tim(h, m, s);
			min = KGlobal::locale()->formatTime(tim, true);
			// FIXME: how to strip hours from returned string?
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
void TopLevel::setToolTip(const QString &text)
{
	if (lastTip == text)
		return;
	lastTip = text;
	QToolTip::remove(this);
	QToolTip::add(this, text);
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
	QValueVector<QString>::ConstIterator ti = times.begin();
	for (QValueVector<QString>::ConstIterator it = teas.begin(); it != teas.end(); it++,ti++) {
		// construct string with name and steeping time
		QString str = *it;
		uint total_seconds = (*ti).toUInt();
		str.append(" (");
		if (total_seconds / 60)
			str.append(i18n("%1min").arg(total_seconds / 60));
		if (total_seconds % 60)
			str.append(i18n("%1s").arg(total_seconds % 60));
		str.append(")");

		start_menu->insertItem(str, id, index);     // add to left-click menu
		menu->insertItem(str, id++, index++);       // add to right-click menu
	}

	// now select 'current' tea
	if (current_selected >= (int)teas.count())
		current_selected = -1;
	for (unsigned int i=0; i < teas.count(); i++)
		menu->setItemChecked(i, (int)i == current_selected);
}

/* enable/disable menu-entries according to current running-state */
void TopLevel::enable_menuEntries()
{
	for (int index=0; menu->idAt(index) >= 0; index++) {
		// [en|dis]able all tea-entries (all have positive menu-ids)
		menu->setItemEnabled(menu->idAt(index), !running);
	}

	startAct->setEnabled(!running);     // "start" entry
	stopAct->setEnabled(running);       // "stop" entry
	confAct->setEnabled(!running);      // "configuration" entry
}

/* menu-slot: tea selected in tea-menu */
void TopLevel::teaSelected(int index)
{
	if (index >=0 && index < (int)teas.count()) {
		// tick new active item in +both+ menus
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
	if (index >=0 && index < (int)teas.count()) {
		teaSelected(index);

		start();
	}
}

/* menu-slot: "start" selected in menu */
void TopLevel::start()
{
	if (current_selected >= 0)
	{
		current_name = teas[current_selected];          // remember name of current tea
		seconds = times[current_selected].toInt();      // initialize time for current tea

		killTimers();
		startTimer(1000);

		running = true;
		ready = false;
		enable_menuEntries();                           // disable "start", enable "stop"

		repaint();
	}
	else
		KMessageBox::error(this, i18n("There is no tea to begin steeping."), i18n("No Tea"));
}

/* menu-slot: "stop" selected in menu */
void TopLevel::stop()
{
	killTimers();

	running = false;
	ready = false;
	enable_menuEntries();                               // disable "top", enable "start"

	setToolTip(i18n("The Tea Cooker"));
	repaint();
}


//
// Configure-window handling
//


/* enable/disable buttons for editing listbox */
void TopLevel::enable_controls() {
	bool amFirst = (listbox->currentItem() == listbox->firstChild());
	bool amLast = (listbox->currentItem() == listbox->lastItem());
	bool haveSelection = (listbox->currentItem() != 0);

	btn_del->setEnabled(haveSelection);
	btn_up->setEnabled(haveSelection && !amFirst);
	btn_down->setEnabled(haveSelection && !amLast);
	if (haveSelection) {
		listbox->ensureItemVisible(listbox->currentItem());
	}
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
	if (listbox->currentItem())
	{
		// item selected, display its properties on right side
		nameEdit->setText(listbox->currentItem()->text(0));
		timeEdit->setValue(listbox->currentItem()->text(1).toInt());
		enable_controls();
	}
}

/* config-slot: name of a tea edited */
void TopLevel::nameEditTextChanged(const QString& newText) {
	listbox->blockSignals(TRUE);
	listbox->currentItem()->setText(0, newText);
	listbox->blockSignals(FALSE);
}

/* config-slot: time for a tea changed */
void TopLevel::spinBoxValueChanged(const QString& newText) {
	QString str = newText.left(newText.length() - timeEdit->suffix().length());  // strip suffix
	listbox->currentItem()->setText(1, str);
}

/* config-slot: "new" button clicked */
void TopLevel::newButtonClicked() {
	QListViewItem* item = new QListViewItem(listbox, listbox->currentItem());
	listbox->setCurrentItem(item);

	nameEdit->setText(i18n("New Tea"));
	timeEdit->setValue(60);

	nameEdit->setFocus();

	if (listbox->childCount() == 1)
		enable_properties();
	enable_controls();
}

/* config-slot: "delete" button clicked */
void TopLevel::delButtonClicked() {
	if (listbox->currentItem())
	{
		delete listbox->currentItem();

		if (listbox->childCount() == 0)
			disable_properties();
		listbox->setSelected(listbox->currentItem(), true); // select new current item
		enable_controls();
	}
}

/* config-slot: "up" button clicked */
void TopLevel::upButtonClicked() {
	QListViewItem* item = listbox->selectedItem();

	if (item && item->itemAbove())
		item->itemAbove()->moveItem(item);

	enable_controls();
}

/* config-slot: "down" button clicked */
void TopLevel::downButtonClicked() {
	QListViewItem* item = listbox->selectedItem();

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


void TopLevel::config()
{
  KDialogBase *dlg = new KDialogBase(KDialogBase::Plain, i18n("Configure The Tea Cooker"),
				     KDialogBase::Ok|KDialogBase::Cancel|KDialogBase::Help,
				     KDialogBase::Ok, this, "config", true);
  QWidget *page = dlg->plainPage();
  // FIXME: enforce sensible initial/default size of dialog
  // FIXME: modal is ok, but can avoid always-on-top?

  QBoxLayout *top_box = new QVBoxLayout(page, 0, 8);	// whole config-stuff
  QBoxLayout *box = new QHBoxLayout(top_box);		// list + properties

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

  // now buttons for constructing tea-list
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
  btn_del->setPixmap(SmallIcon("editdelete"));
  btn_del->setMinimumSize(btn_new->sizeHint() * 1.2);
  connect(btn_del, SIGNAL(clicked()), SLOT(delButtonClicked()));
  hbox->addWidget(btn_del);

  btn_up = new QPushButton(QString::null, listgroup_widget);
  QToolTip::add(btn_up, i18n("Up"));
  btn_up->setPixmap(SmallIcon("up"));
  btn_up->setMinimumSize(btn_up->sizeHint() * 1.2);
  connect(btn_up, SIGNAL(clicked()), SLOT(upButtonClicked()));
  hbox->addWidget(btn_up);

  btn_down = new QPushButton(QString::null, listgroup_widget);
  QToolTip::add(btn_down, i18n("Down"));
  btn_down->setPixmap(SmallIcon("down"));
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
  nameEdit->setAlignment(QLineEdit::AlignRight);
  QLabel *l = new QLabel(nameEdit, i18n("Name:"), propleft);
  l->setFixedSize(l->sizeHint());
  connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(nameEditTextChanged(const QString&)) );

  timeEdit = new KIntSpinBox(1, 10000, 10, 1, 10, propright);
  timeEdit->setSuffix(i18n(" s"));
  timeEdit->setFixedHeight(timeEdit->sizeHint().height());
  l = new QLabel(timeEdit, i18n("Tea time:"), propleft);
  l->setFixedSize(l->sizeHint());
  connect(timeEdit, SIGNAL(valueChanged(const QString&)), SLOT(spinBoxValueChanged(const QString&)) );

  /* bottom - timeout actions */
  QGroupBox *actiongroup = new QGroupBox(3, Vertical, i18n("Action"), page);
  top_box->addWidget(actiongroup, 0, 0);

  QHBox *actionbox = new QHBox(actiongroup);
  QCheckBox *actionEnable = new QCheckBox(actionbox);
  actionEdit = new QLineEdit(actionbox);
  actionEdit->setFixedHeight(actionEdit->sizeHint().height());
  QToolTip::add(actionEdit, i18n("%t: Tea Sort"));
  connect(actionEnable, SIGNAL(toggled(bool)), SLOT(actionEnableToggled(bool)));

  QCheckBox *beep = new QCheckBox(i18n("Beep"), actiongroup);
  QCheckBox *popup = new QCheckBox(i18n("Popup"), actiongroup);
  beep->setFixedHeight(beep->sizeHint().height());
  popup->setFixedHeight(popup->sizeHint().height());
  rightside->addStretch();

  // let listbox claim all remaining vertical space
  top_box->setStretchFactor(box, 10);

  // now add all defined teas (and their times) to the listview
  // this is done backwards because QListViewItems are inserted at the end
  for (int i=teas.count()-1; i>=0; i--)
  {
    QListViewItem* item = new QListViewItem(listbox);
    item->setText(0, teas[i]);
    item->setText(1, times[i]);
    if (i == current_selected)
      listbox->setSelected(item, true);
  }
  // select first entry in listbox; if no entries present then disable right side
  if (listbox->childCount() == 0) {
    enable_controls();
    disable_properties();
  }

  connect(dlg, SIGNAL(helpClicked()), SLOT(help()));

  // -------------------------

  beep->setChecked(beeping);
  popup->setChecked(popping);
  actionEnable->setChecked(useAction);
  actionEdit->setText(action);
  actionEdit->setEnabled(useAction);

  if (dlg->exec() == QDialog::Accepted)
  {
    // activate new settings
    beeping = beep->isChecked();
    popping = popup->isChecked();
    useAction = actionEnable->isChecked();
    action = actionEdit->text();

    teas.clear();
    times.clear();

    // Copy over teas and times from the QListView
    int i = 0;
    current_name = QString::null;
    current_selected = 0;
	times.clear();
	teas.clear();
	times.resize(listbox->childCount());
	teas.resize(listbox->childCount());
    for (QListViewItemIterator it(listbox); it.current() != 0; it++)
    {
      teas[i] = it.current()->text(0);
      times[i] = it.current()->text(1);
      if (it.current()->isSelected())
      {
        current_selected = i;
        current_name = it.current()->text(0);
      }
      i++;
    }
    if (current_selected == 0) {
      if (teas.empty())
        current_selected = -1;
      else
        current_name = *(teas.begin());
    }

    rebuildTeaMenus();

    // and store to config
    KConfig *config = kapp->config();
    // remove old-style entries from default-group (if present)
    if (config->hasKey("UserTea"))
      config->deleteEntry("UserTea");

    config->setGroup("General");
    config->writeEntry("Beep", beeping);
    config->writeEntry("Popup", popping);
    config->writeEntry("UseAction", useAction);
    config->writeEntry("Action", action);
    config->writeEntry("Tea", current_selected);
    // first get rid of all previous tea-entries from config, then write anew
    config->deleteGroup("Teas", true);			// deep remove of whole group
    config->setGroup("Teas");
    config->writeEntry("Number", teas.count());
    QString key;
	QValueVector<QString>::ConstIterator ti = times.begin();
    int index = 1;
	for (QValueVector<QString>::ConstIterator it = teas.begin(); it != teas.end(); it++) {
      key.sprintf("Tea%d Name", index);
      config->writeEntry(key, *it);
      key.sprintf("Tea%d Time", index);
      config->writeEntry(key, *ti);
      ti++;
      index++;
    }

    config->sync();
  }
}
