/* -------------------------------------------------------------

   toplevel.cpp

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */

#include <stdlib.h>

#include <qcheckbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenudata.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qfile.h>

#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kwin.h>
#include <knuminput.h>
#include <kseparator.h> 

#include "toplevel.h"
#include <kdialogbase.h>

#include "mug.xpm"
#include "bag.xpm"
#include "tea1.xpm"
#include "tea2.xpm"

TopLevel::TopLevel()
    : KSystemTray()
{
  setBackgroundMode(X11ParentRelative);
  QString n;
  unsigned int num;

  menu = new QPopupMenu();
  menu->setCheckable(true);

  KConfig *config = kapp->config();
  config->setGroup("Teas");

  teas.clear(); times.clear();

  teas.append(i18n("Black Tea (3 min)")); n.setNum(180); times.append(n);
  teas.append(i18n("Earl Grey (5 min)")); n.setNum(300); times.append(n);
  teas.append(i18n("Fruit Tea (8 min)")); n.setNum(480); times.append(n);

  num = config->readNumEntry("UserTea", 150);
  n = i18n("Other tea (%1s)").arg(num);
  teas.append(n); n.setNum(num); times.append(n);

  int index = 0;
  for (QStringList::ConstIterator it = teas.begin(); it != teas.end(); it++)
    menu->insertItem(*it, index++);

  connect(menu, SIGNAL(activated(int)), this, SLOT(teaSelected(int)));

  current_tea = num = config->readNumEntry("Tea",0);
  if (num > teas.count())
    num = 0;
  for (unsigned int i=0; i < teas.count(); i++)
    menu->setItemChecked(i, i == num);
  teatime = (*times.at(num)).toInt();

  menu->insertSeparator();
  menu->insertItem(SmallIcon("1rightarrow"), i18n("&Start"), this, SLOT(start()));
  menu->insertSeparator();
  menu->insertItem(SmallIcon("configure"), i18n("&Configure..."), this, SLOT(config()));
  menu->insertItem(SmallIcon("exit"), i18n("Quit"), kapp, SLOT(quit()));

  beeping = config->readBoolEntry("Beep", true);
  popping = config->readBoolEntry("Popup", true);
  action = config->readEntry("Action");

  mugPixmap = new QPixmap(mug);
  bagPixmap = new QPixmap(bag);
  tea1Pixmap = new QPixmap(tea1);
  tea2Pixmap = new QPixmap(tea2);

  running = ready = false;

  setToolTip(i18n("The Tea Cooker"));
}


TopLevel::~TopLevel()
{
  delete menu;
  delete mugPixmap;
  delete bagPixmap;
  delete tea1Pixmap;
  delete tea2Pixmap;
}


void TopLevel::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == 1)
  {
    if (ready)
    {
      killTimers();
      ready = false;
      repaint();
    }
    else
      if (!running)
        start();
  }
  else
    menu->popup(QCursor::pos());
}


void TopLevel::paintEvent(QPaintEvent *)
{
  QPixmap *pm;

  if (running)
    pm = bagPixmap;
  else
  {
    if (ready)
    {
      if (frame1)
        pm = tea1Pixmap;
      else
        pm = tea2Pixmap;
    }
    else
      pm = mugPixmap;
  }

  QPainter p(this);
  int x = 1 + (12 - pm->width()/2);
  int y = 1 + (12 - pm->height()/2);
  p.drawPixmap(x , y, *pm);
  p.end();
}


void TopLevel::start()
{
  killTimers();
  seconds = teatime;
  startTimer(1000);

  running = true; ready = false;
  repaint();
}


void TopLevel::timerEvent(QTimerEvent *)
{
  if (running)
  {
    seconds--;

    if (seconds <= 0)
    {
      running = false; ready = true;

      // invoke action
      if (beeping)
        KNotifyClient::beep();
      if (!action.isEmpty())
        system(QFile::encodeName(action));
      if (popping)
          KMessageBox::information(0, i18n("The tea is now ready!"));
      setToolTip(i18n("The tea is now ready!"));
      repaint();
    }
    else {
        QString min;
        min.sprintf("%.1f", (seconds / 12) / 5.);
        setToolTip(i18n("%1 minutes left").arg(min));
    }
  }
  else
    if (ready)
    {
        frame1 = !frame1;
        repaint();
    }
}


void TopLevel::teaSelected(int index)
{

  if (index >=0 && index < (int)teas.count())
  {
    for (unsigned int i=0; i < teas.count(); i++)
      menu->setItemChecked(i, (int)i == index);

    KConfig *config = kapp->config();
    config->setGroup("Teas");

    config->writeEntry("Tea", index);

    bool ok;
    teatime = (*times.at(index)).toInt(&ok);
    if (!ok)
      teatime = 300;
    current_tea = index;
  }
}


void TopLevel::config()
{
  KDialogBase *dlg = new KDialogBase(KDialogBase::Plain, i18n("Configure The Tea Cooker"),
                                     KDialogBase::Ok|KDialogBase::Cancel|KDialogBase::Help,
                                     KDialogBase::Ok,
                                     this, "config", true);
  QWidget *page = dlg->plainPage();

  QVBoxLayout *box = new QVBoxLayout(page,4,8);

  QGridLayout *grid = new QGridLayout(5,2);
  box->addLayout(grid);

  KIntSpinBox *spin = new KIntSpinBox(1,10000,10, 1, 10, page);
  spin->setFixedHeight(spin->sizeHint().height());

  QLabel *l = new QLabel(spin, i18n("Your Tea Time (s):"), page);
  l->setFixedSize(l->sizeHint());

  grid->addWidget(l,0,0);
  grid->addWidget(spin,0,1);

  QLineEdit *actionEdit = new QLineEdit(page);
  l = new QLabel(actionEdit, i18n("Action:"), page);
  l->setMinimumSize(l->sizeHint());
  actionEdit->setFixedHeight(actionEdit->sizeHint().height());

  grid->addWidget(l,1,0);
  grid->addWidget(actionEdit,1,1);

  QCheckBox *beep = new QCheckBox(i18n("Beep"), page);
  QCheckBox *popup = new QCheckBox(i18n("Popup"), page);
  beep->setFixedHeight(beep->sizeHint().height());
  popup->setFixedHeight(popup->sizeHint().height());

  grid->addWidget(beep, 2,1);
  grid->addWidget(popup, 3,1);

  grid->setRowStretch(4,1);

  KSeparator *hline = new KSeparator( KSeparator::HLine, page);
  box->addStretch(1);
  box->addWidget(hline);

  connect(dlg, SIGNAL(helpClicked()), SLOT(help()));

  // -------------------------

  beep->setChecked(beeping);
  popup->setChecked(popping);
  actionEdit->setText(action);
  bool done;
  int num = (*times.at(teas.count()-1)).toInt(&done);
  if (!done)
      num = 300;
  spin->setValue(num);

  if (dlg->exec() == QDialog::Accepted)
  {
    beeping = beep->isChecked();
    popping = popup->isChecked();
    action = actionEdit->text();
    teas.remove(teas.last());
    times.remove(times.last());

    QString n,n2;
    num = spin->value();
    n = i18n("Other tea (%1s)").arg(num);
    teas.append(n); n2.setNum(num); times.append(n2);
    menu->changeItem(n, teas.count()-1);
    if (current_tea == (int)teas.count() - 1)
      teatime = num;

    KConfig *config = kapp->config();
    config->setGroup("Teas");
    config->writeEntry("Beep",beeping);
    config->writeEntry("Popup",popping);
    config->writeEntry("UserTea",num);
    config->writeEntry("Action",action);
    config->sync();
  }
}

void TopLevel::help()
{
    kapp->invokeHelp();
}

void TopLevel::setToolTip(const QString &text)
{
    if (lasttip == text)
        return;
    lasttip = text;
    QToolTip::remove(this);
    QToolTip::add(this, text);
}
