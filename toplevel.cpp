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
#include "toplevel.h"
#include "toplevel.moc"

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

	steeping_menu = new QPopupMenu();
	steeping_menu->insertItem(SmallIcon("cancel"), i18n("Just &cancel current"), this, SLOT(stop()));

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


TopLevel::~TopLevel()
{
	delete menu;
	delete mugPixmap;
	delete teaNotReadyPixmap;
	delete teaAnim1Pixmap;
	delete teaAnim2Pixmap;
	delete steeping_menu;
	// FIXME: must delete more (like all the QWidgets in config-window)?
}


void TopLevel::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == LeftButton) {
		if (ready) {
			stop();			// reset tooltip and stop animation
		} else {
			if (running)
				steeping_menu->popup(QCursor::pos());
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


void TopLevel::timerEvent(QTimerEvent *)
{
	if (running) {
		seconds--;

		if (seconds <= 0) {
			running = false;
			ready = true;
			enable_menuEntries();

			QString teaMessage =
			    i18n("The %1 is now ready!").arg(current_name);
			// invoke action
			if (beeping)
				KNotifyClient::beep();
			if (useAction && (!action.isEmpty()))
				system(QFile::encodeName(action));
			if (popping)
				KPassivePopup::message(i18n("The Tea Cooker"),
				                       teaMessage, *teaAnim1Pixmap, this, "popup", 0);
				// FIXME: does auto-deletion work without timeout?
			setToolTip(teaMessage);
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
			firstFrame = !firstFrame;
			repaint();
		}
	}
}

void TopLevel::setToolTip(const QString &text)
{
	if (lastTip == text)
        	return;
	lastTip = text;
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
		uint total_seconds = (*ti).toUInt();
                str.append(" (");
		if (total_seconds / 60)
		{
		  str.append(i18n("%1min").arg(total_seconds / 60));
		}
		if (total_seconds % 60)
		{
		  str.append(i18n("%1s").arg(total_seconds % 60));
		}
                str.append(")");
		menu->insertItem(str, id++, index++);
		ti++;
	}

	// now select 'current' tea
	if (current_selected >= teas.count())
		current_selected = -1;
	for (unsigned int i=0; i < teas.count(); i++)
		menu->setItemChecked(i, i == current_selected);
}

/* enable/disable "start" and "stop" menu-entries according to current running-state */
void TopLevel::enable_menuEntries()
{
	int index = 0;
	while (menu->idAt(index) >= 0){     // find first non-positive menu-id
		menu->setItemEnabled(menu->idAt(index), !running);
		index++;
	}
	index += 1;                         // skip separator

	menu->setItemEnabled(menu->idAt(index), !running);      // "start" entry
	menu->setItemEnabled(menu->idAt(index+1), running);     // "stop" entry
	menu->setItemEnabled(menu->idAt(index+3), !running);    // "configuration" entry
}

/* menu-slot: tea selected in tea-menu */
void TopLevel::teaSelected(int index)
{
	if (index >=0 && index < (int)teas.count()) {
		menu->setItemChecked(current_selected, false);
		menu->setItemChecked(index, true);

		current_selected = index;
		KConfig *config = kapp->config();
		config->setGroup("General");
		config->writeEntry("Tea", current_selected);
	}
}

/* menu-slot: "start" selected in menu */
void TopLevel::start()
{
	if (current_selected >= 0)
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
	else
		KMessageBox::error(this, i18n("There is no tea to begin steeping."), i18n("No Tea"));
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
	QListViewItem* item = new QListViewItem(listbox, 0);
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

  QWidget *listgroup_widget = new QWidget(listgroup);
  QBoxLayout *listgroup_layout = new QVBoxLayout(listgroup_widget);

  listbox = new QListView(listgroup_widget, "listBox");
  listbox->addColumn(i18n("Name"));
  listbox->header()->setClickEnabled(false, listbox->header()->count()-1);
  listbox->addColumn(i18n("Time"));
  listbox->header()->setClickEnabled(false, listbox->header()->count()-1);
  listgroup_layout->addWidget(listbox);

  // now add all defined teas (and their times) to the listview
  // this is done backwards because QListViewItem's are inserted at the end
  QStringList::ConstIterator ti = times.begin();
  for (QStringList::ConstIterator it = teas.begin(); it != teas.end(); it++, ti++)
  {
	QListViewItem* item = new QListViewItem(listbox);
	item->setText(0, *it);
	item->setText(1, *ti);
  }

  connect(listbox, SIGNAL(selectionChanged()), SLOT(listBoxItemSelected()));

  // now buttons for constructing tea-list
  QBoxLayout *hbox = new QHBoxLayout(listgroup_layout);
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
  (void) new QLabel(nameEdit, i18n("Name:"), propleft);
  connect(nameEdit, SIGNAL(textChanged(const QString&)), SLOT(nameEditTextChanged(const QString&)) );

  timeEdit = new KIntSpinBox(1, 10000, 10, 1, 10, propright);
  timeEdit->setSuffix(i18n(" s"));
  timeEdit->setFixedHeight(timeEdit->sizeHint().height());
  QLabel *l = new QLabel(timeEdit, i18n("Tea time:"), propleft);
  l->setFixedSize(l->sizeHint());
  connect(timeEdit, SIGNAL(valueChanged(const QString&)), SLOT(spinBoxValueChanged(const QString&)) );

  /* bottom - timeout actions */
  QGroupBox *actiongroup = new QGroupBox(3, Vertical, i18n("Action"), page);
  top_box->addWidget(actiongroup, 0, 0);

  QHBox *actionbox = new QHBox(actiongroup);
  QCheckBox *actionEnable = new QCheckBox(actionbox);
  actionEdit = new QLineEdit(actionbox);
  actionEdit->setFixedHeight(actionEdit->sizeHint().height());
  connect(actionEnable, SIGNAL(toggled(bool)), SLOT(actionEnableToggled(bool)));

  QCheckBox *beep = new QCheckBox(i18n("Beep"), actiongroup);
  QCheckBox *popup = new QCheckBox(i18n("Popup"), actiongroup);
  beep->setFixedHeight(beep->sizeHint().height());
  popup->setFixedHeight(popup->sizeHint().height());
  rightside->addStretch();

  // let listbox claim all remaining vertical space
  top_box->setStretchFactor(box, 10);

  // select first entry in listbox, if no entries then disable right side
  if (listbox->childCount() > 0) {
  	listbox->setSelected(listbox->firstChild(), true);
  } else {
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
    for (QListViewItemIterator it(listbox); it.current() != 0; it++)
    {
      teas.append(it.current()->text(0));
      times.append(it.current()->text(1));
      if (it.current()->isSelected())
      {
        current_selected = i;
        current_name = it.current()->text(0);
      }
      i++;
    }
    if (current_selected == 0)
      if (teas.empty())
        current_selected = -1;
      else
        current_name = *(teas.begin());

    rebuildTeaMenu();

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
