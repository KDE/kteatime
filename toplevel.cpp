/* -------------------------------------------------------------

   toplevel.cpp

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

       Some additions by Martin Willers <willers@xm-arts.de>, 2002

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

#include <kconfig.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <knuminput.h>
#include <kseparator.h>
#include <kpopupmenu.h>
#include "toplevel.h"
#include "toplevel.moc"
#include <kdialogbase.h>

#include "mug.xpm"
#include "bag.xpm"
#include "tea1.xpm"
#include "tea2.xpm"



TopLevel::TopLevel() : KSystemTray()
{
	setBackgroundMode(X11ParentRelative);
	QString n, key;
	unsigned int num;

	teas.clear();
	times.clear();

	KConfig *config = kapp->config();
	config->setGroup("Teas");

	if (config->hasKey("Number")) {
		// assuming this is a new-style config
		num = config->readNumEntry("Number", 0);
		for (unsigned int index=1; index<=num; index++) {
			key.sprintf("Tea%d Time", index);
			times.append(config->readEntry(key, NULL));
  			key.sprintf("Tea%d Name", index);
			teas.append(config->readEntry(key, NULL));
  		}
		config->setGroup("General");
	} else {
		// either old-style config or first start, so provide some sensible defaults
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
	// sanity-check for this is done on rebuildTeaMenu()


	// create app menu
	menu = new QPopupMenu();
	menu->setCheckable(true);
	connect(menu, SIGNAL(activated(int)), this, SLOT(teaSelected(int)));

	rebuildTeaMenu();		// populate top of menu with tea-entries from config

	KHelpMenu* help = new KHelpMenu(this, KGlobal::instance()->aboutData(), false);
	KPopupMenu* helpMnu = help->menu();

	menu->insertSeparator();
	menu->insertItem(SmallIcon("1rightarrow"), i18n("&Start"), this, SLOT(start()));
	menu->insertItem(SmallIcon("cancel"), i18n("Sto&p"), this, SLOT(stop()));
	menu->insertSeparator();
	menu->insertItem(SmallIcon("configure"), i18n("&Configure..."), this, SLOT(config()));
	menu->insertItem(SmallIcon("help"), i18n("&Help"), helpMnu);
	menu->insertItem(SmallIcon("exit"), i18n("Quit"), kapp, SLOT(quit()));

	beeping = config->readBoolEntry("Beep", true);
	popping = config->readBoolEntry("Popup", true);
	action = config->readEntry("Action");

	mugPixmap = new QPixmap(mug);
	bagPixmap = new QPixmap(bag);
	tea1Pixmap = new QPixmap(tea1);
	tea2Pixmap = new QPixmap(tea2);

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


TopLevel::~TopLevel()
{
	delete menu;
	delete mugPixmap;
	delete bagPixmap;
	delete tea1Pixmap;
	delete tea2Pixmap;
	// FIXME: must delete more (like all the QWidgets in config-window)?
}


void TopLevel::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == LeftButton) {
		if (ready) {
			stop();			// reset tooltip and stop animation
		} else {
			if (running)
				stop();
			else if (!running)
				start();
		}
	} else if (event->button() == RightButton)
		menu->popup(QCursor::pos());
//	else if (event->button() == MidButton)	// currently unused
}


void TopLevel::paintEvent(QPaintEvent *)
{
	QPixmap *pm;

	if (running)
		pm = bagPixmap;
	else {
		if (ready) {
			if (frame1)
				pm = tea1Pixmap;
			else
				pm = tea2Pixmap;
		} else
			pm = mugPixmap;
	}

	QPainter p(this);
	int x = 1 + (12 - pm->width()/2);
	int y = 1 + (12 - pm->height()/2);
	p.drawPixmap(x , y, *pm);
	p.end();
}


void TopLevel::timerEvent(QTimerEvent *)
{
	if (running) {
		seconds--;

		if (seconds <= 0) {
			running = false;
			ready = true;
			enable_menuEntries();

			// invoke action
			if (beeping)
				KNotifyClient::beep();
			if (!action.isEmpty())
				system(QFile::encodeName(action));
			if (popping)
				KMessageBox::information(0, i18n("The %1 is now ready!").arg(current_name));
			setToolTip(i18n("The %1 is now ready!").arg(current_name));
			repaint();
		} else {
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
		if (ready) {
			frame1 = !frame1;
			repaint();
		}
	}
}

void TopLevel::setToolTip(const QString &text)
{
	if (lasttip == text)
        	return;
	lasttip = text;
	QToolTip::remove(this);
	QToolTip::add(this, text);
}

void TopLevel::rebuildTeaMenu() {
	// first remove all current tea-entries from menu, these can be identified by their positive id
	while (menu->idAt(0) >= 0)
		menu->removeItemAt(0);

	// now add new tea-entries to top of menu
	int id = 0;
	int index = 0;
	QStringList::ConstIterator ti = times.begin();
	for (QStringList::ConstIterator it = teas.begin(); it != teas.end(); it++) {
		QString str = *it;
		str.append(" (");
		uint total_seconds = (*ti).toUInt();
		QString temp;
		if (total_seconds / 60)
		{
		  temp.setNum(total_seconds / 60);
		  str.append(temp+"m");
		}
		if (total_seconds % 60)
		{
		  temp.setNum(total_seconds % 60);
		  str.append(temp+"s");
		}
		str.append(")");
		menu->insertItem(str, id++, index++);
		ti++;
	}

	// now select 'current' tea
	// FIXME: does it make sense to assume a valid 'current' tea after reconstruction?
	if (current_selected >= teas.count())
		current_selected = 0;
	for (unsigned int i=0; i < teas.count(); i++)
		menu->setItemChecked(i, i == current_selected);
}

/* enable/disable "start" and "stop" menu-entries according to current running-state */
void TopLevel::enable_menuEntries()
{
	int startindex = 0;
	while (menu->idAt(startindex) >= 0)			// find first non-positive menu-id
		startindex += 1;
	startindex += 1;					// skip separator
	int stopindex = startindex + 1;

	menu->setItemEnabled(menu->idAt(startindex), !running);	// "start" entry
	menu->setItemEnabled(menu->idAt(stopindex), running);	// "stop" entry
}

/* menu-slot: tea selected in tea-menu */
void TopLevel::teaSelected(int index)
{
	if (index >=0 && index < (int)teas.count()) {
		for (unsigned int i=0; i < teas.count(); i++)
			menu->setItemChecked(i, (int)i == index);

		current_selected = index;
		KConfig *config = kapp->config();
		config->setGroup("General");
		config->writeEntry("Tea", current_selected);
	}
}

/* menu-slot: "start" selected in menu */
void TopLevel::start()
{
	current_name = *teas.at(current_selected);		// remember name of current tea
	seconds = (*times.at(current_selected)).toInt();	// initialize time for current tea

	killTimers();
	startTimer(1000);

	running = true;
	ready = false;
	enable_menuEntries();					// disable "start", enable "stop"

	repaint();
}

/* menu-slot: "stop" selected in menu */
void TopLevel::stop()
{
	killTimers();

	running = false;
	ready = false;
	enable_menuEntries();					// disable "top", enable "start"

	setToolTip(i18n("The Tea Cooker"));
	repaint();
}


//
// Configure-window handling
//


/* enable/disable buttons for editing listbox */
void TopLevel::enable_controls() {
	bool amFirst = (listbox->currentItem() == 0);
	bool amLast = (listbox->currentItem() == listbox->count() - 1);
	bool haveSelection = (listbox->currentItem() >= 0);

	btn_del->setEnabled(haveSelection);
	btn_up->setEnabled(haveSelection && !amFirst);
	btn_down->setEnabled(haveSelection && !amLast);
	if (haveSelection) {
		listbox->ensureCurrentVisible();
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
void TopLevel::listBoxItemSelected(int id) {
	if (id >= 0) {
  		// item selected, display its properties on right side
  		nameEdit->setText(listbox->item(id)->text());
  		timeEdit->setValue((*ntimes.at(id)).toInt());
	} else {
  		// no item selected anymore, so clear right side
		nameEdit->setText("");
		timeEdit->setValue(1);
	}
	enable_controls();
}

/* config-slot: name of a tea edited */
void TopLevel::nameEditTextChanged(const QString& newText) {
	int index = listbox->currentItem();
	listbox->blockSignals(TRUE);
	listbox->changeItem(newText, index);
	listbox->blockSignals(FALSE);
}

/* config-slot: time for a tea changed */
void TopLevel::spinBoxValueChanged(const QString& newText) {
	int index = listbox->currentItem();
	if (index != -1) {
		QString str = newText.left(newText.length() - timeEdit->suffix().length());  // strip suffix
  		QStringList::Iterator it = ntimes.at(index);
		*it = str;
	}
}

/* config-slot: "new" button clicked */
void TopLevel::newButtonClicked() {
	int pos = listbox->currentItem();
	listbox->insertItem(i18n("New Tea"), pos);		// if pos<0, insertItem will append
	if (pos < 0) {
  		ntimes.append(QString::number(1));			// add time
		listbox->setSelected(listbox->count()-1, true);		// select new entry
	} else {
		ntimes.insert(ntimes.at(pos), QString::number(1));	// add time
		listbox->setSelected(pos, true);			// select new entry
	}
	// FIXME: also give focus to nameEdit (how?)
	if (listbox->count() == 1)
		enable_properties();
	enable_controls();
}

/* config-slot: "delete" button clicked */
void TopLevel::delButtonClicked() {
	int index = listbox->currentItem();
	if (index != -1) {
		// remove Time for this tea first, so listbox-update gets it right
		QStringList::Iterator it = ntimes.at(index);
		ntimes.remove(it);
  		listbox->removeItem(index);
		if (listbox->count() == 0)
			disable_properties();
		enable_controls();
		// FIXME: if 'current_selected' tea has been deleted, reset current_selected to ?
	}
}

/* config-slot: "up" button clicked */
void TopLevel::upButtonClicked() {
	int index = listbox->currentItem();
	if (index != -1) {
  		// selected item is at index, move up
  		QListBoxItem *item = listbox->item(index-1);
		listbox->takeItem(item);
		listbox->insertItem(item, index);
		// also move Time!
		QStringList::Iterator it = ntimes.at(index);
		QString str = *it;
		*it = *ntimes.at(index-1);
		*ntimes.at(index-1) = str;
		enable_controls();
	}
}

/* config-slot: "down" button clicked */
void TopLevel::downButtonClicked() {
	int index = listbox->currentItem();
	if (index != -1) {
  		// selected item is at index, move down
  		QListBoxItem *item = listbox->item(index+1);
		listbox->takeItem(item);
		listbox->insertItem(item, index);
		// also move Time!
		QStringList::Iterator it = ntimes.at(index);
		QString str = *it;
		*it = *ntimes.at(index+1);
		*ntimes.at(index+1) = str;
		enable_controls();
	}
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

  // FIXME: currently, teas are modified inside QListBox, but times are not,
  //        so I need a copy of times to work with
  //        --> maybe display (and work with) times in QListBox, too?
  ntimes = times;

  /* left side - tea list and list-modifying buttons */
  QBoxLayout *leftside = new QVBoxLayout(box);
  QGroupBox *listgroup = new QGroupBox(2, Vertical, i18n("Tea List"), page);
  leftside->addWidget(listgroup, 0, 0);
  listbox = new QListBox(listgroup, "listBox");
  // now add all defined teas to listbox
//  QStringList::ConstIterator ti = times.begin();
  // FIXME: use a two-column listbox and display time there also?
  for (QStringList::ConstIterator it = teas.begin(); it != teas.end(); it++) {
	QString str = *it;
	listbox->insertItem(str);
//	ti++;
  }
  connect(listbox, SIGNAL(highlighted(int)), SLOT(listBoxItemSelected(int)));

  // now buttons for constructing tea-list
  QHBox *hbox = new QHBox(listgroup);
  hbox->setSpacing(4);
  btn_new = new QPushButton(QString::null, hbox);
  QToolTip::add(btn_new, i18n("New"));
  btn_new->setPixmap(SmallIcon("filenew"));
  btn_new->setMinimumSize(btn_new->sizeHint() * 1.2);
  connect(btn_new, SIGNAL(clicked()), SLOT(newButtonClicked()));
  btn_del = new QPushButton(QString::null, hbox);
  QToolTip::add(btn_del, i18n("Delete"));
  btn_del->setPixmap(SmallIcon("editdelete"));
  btn_del->setMinimumSize(btn_new->sizeHint() * 1.2);
  connect(btn_del, SIGNAL(clicked()), SLOT(delButtonClicked()));
  btn_up = new QPushButton(QString::null, hbox);
  QToolTip::add(btn_up, i18n("Up"));
  btn_up->setPixmap(SmallIcon("up"));
  btn_up->setMinimumSize(btn_up->sizeHint() * 1.2);
  connect(btn_up, SIGNAL(clicked()), SLOT(upButtonClicked()));
  btn_down = new QPushButton(QString::null, hbox);
  QToolTip::add(btn_down, i18n("Down"));
  btn_down->setPixmap(SmallIcon("down"));
  btn_down->setMinimumSize(btn_down->sizeHint() * 1.2);
  connect(btn_down, SIGNAL(clicked()), SLOT(downButtonClicked()));
// FIXME: somehow add stretch-space now to avoid stretching of buttons
//  hbox->addStretch(10);	// this doesn't work with QHBox...

  /* right side - tea properties */
  QBoxLayout *rightside = new QVBoxLayout(box);
  editgroup = new QGroupBox(2, Vertical, i18n("Tea Properties"), page);
  rightside->addWidget(editgroup, 0, 0);
  QHBox *propbox = new QHBox(editgroup);

  // FIXME: - must enforce correct vertical alignment of each label-editor pair
  //          (better use one HBox for each label-editor pair?)
  // FIXME: - right-justify text in labels (how?)
  QVBox *propleft = new QVBox(propbox);
  QVBox *propright = new QVBox(propbox);
  nameEdit = new QLineEdit(propright);
  nameEdit->setFixedHeight(nameEdit->sizeHint().height());
  (void) new QLabel(nameEdit, i18n("Name:"), propleft);
  connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(nameEditTextChanged(const QString&)) );

  timeEdit = new KIntSpinBox(1, 10000, 10, 1, 10, propright);
  timeEdit->setSuffix(i18n(" s"));
  timeEdit->setFixedHeight(timeEdit->sizeHint().height());
  QLabel *l = new QLabel(timeEdit, i18n("Tea Time:"), propleft);
  l->setFixedSize(l->sizeHint());
  connect(timeEdit, SIGNAL(valueChanged(const QString&)), SLOT(spinBoxValueChanged(const QString&)) );

  /* bottom - timeout actions */
  QGroupBox *actiongroup = new QGroupBox(3, Vertical, i18n("Action"), page);
  top_box->addWidget(actiongroup, 0, 0);

  // FIXME: place QCheckBox before QLineEdit, too
  QLineEdit *actionEdit = new QLineEdit(actiongroup);
  actionEdit->setFixedHeight(actionEdit->sizeHint().height());

  QCheckBox *beep = new QCheckBox(i18n("Beep"), actiongroup);
  QCheckBox *popup = new QCheckBox(i18n("Popup"), actiongroup);
  beep->setFixedHeight(beep->sizeHint().height());
  popup->setFixedHeight(popup->sizeHint().height());
  rightside->addStretch();

  // let listbox claim all remaining vertical space
  top_box->setStretchFactor(box, 10);

  // select first entry in listbox, if no entries then disable right side
  if (listbox->count() > 0) {
  	listbox->setSelected(0, true);
  } else {
  	enable_controls();
  	disable_properties();
  }

  connect(dlg, SIGNAL(helpClicked()), SLOT(help()));

  // -------------------------

  beep->setChecked(beeping);
  popup->setChecked(popping);
  actionEdit->setText(action);

  if (dlg->exec() == QDialog::Accepted)
  {
    // activate new settings
    beeping = beep->isChecked();
    popping = popup->isChecked();
    action = actionEdit->text();

    teas.clear();
    for (int i=0; i<listbox->numRows(); i++) {
    	teas.append(listbox->item(i)->text());
    }
    times = ntimes;
    rebuildTeaMenu();

    // and store to config
    KConfig *config = kapp->config();
    // remove old-style entries from default-group (if present)
    if (config->hasKey("UserTea"))
    	config->deleteEntry("UserTea");

    config->setGroup("General");
    config->writeEntry("Beep", beeping);
    config->writeEntry("Popup", popping);
    config->writeEntry("Action", action);
    config->writeEntry("Tea", current_selected);
    // first get rid of all previous tea-entries from config, then write anew
    config->deleteGroup("Teas", true);			// deep remove of whole group
    config->setGroup("Teas");
    config->writeEntry("Number", teas.count());
    QString key;
    QStringList::ConstIterator ti = times.begin();
    int index = 1;
    for (QStringList::ConstIterator it = teas.begin(); it != teas.end(); it++) {
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
