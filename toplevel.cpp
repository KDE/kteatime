/* -------------------------------------------------------------

   toplevel.cpp

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */

#include <qcheckbox.h>
#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlined.h>
#include <qmenudata.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtooltip.h>

#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwm.h>

#include "toplevel.h"

#include "mug.xpm"
#include "bag.xpm"
#include "tea1.xpm"
#include "tea2.xpm"

TopLevel::TopLevel() 
  : KTMainWindow()
{
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
  n.sprintf(i18n("Other tea (%is)"), num);
  teas.append(n); n.setNum(num); times.append(n);

  for (QStringList::ConstIterator it = teas.begin(); it != teas.end(); it++)
    menu->insertItem(*it);

  connect(menu, SIGNAL(activated(int)), this, SLOT(teaSelected(int)));

  num = config->readNumEntry("Tea",0);
  if (num > teas.count())
    num = 0;
  for (unsigned int i=0; i < teas.count(); i++)
    menu->setItemChecked(i, i == num);
  teatime = (*times.at(num)).toInt();

  menu->insertSeparator();
  menu->insertItem(i18n("&Start"), this, SLOT(start()));
  menu->insertSeparator();
  menu->insertItem(i18n("&Configure..."), this, SLOT(config()));
  menu->insertItem(i18n("Quit"), kapp, SLOT(quit()));

  beeping = config->readBoolEntry("Beep", true);
  popping = config->readBoolEntry("Popup", true);
  action = config->readEntry("Action");
    
  mugPixmap = new QPixmap(mug);
  bagPixmap = new QPixmap(bag);
  tea1Pixmap = new QPixmap(tea1);
  tea2Pixmap = new QPixmap(tea2);

  running = ready = false;

  QToolTip::add(this, i18n("The Tea Cooker"));
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
  {
    menu->move(-1000,-1000);
    menu->show();
    menu->hide();
    QRect g = KWM::geometry( this->winId() );
    if ( g.x() > QApplication::desktop()->width()/2 &&
      g.y()+menu->height() > QApplication::desktop()->height() )
      menu->popup(QPoint( g.x(), g.y() - menu->height()));
    else
      menu->popup(QPoint( g.x() + g.width(), g.y() + g.height()));
  }
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
        kapp->beep();
      if (!action.isEmpty())
        system(action.data());
      if (popping)
        KMessageBox::information(0, i18n("The tea is now ready!"));

      repaint();
    }
  }
  else
    if (ready)
    {
      frame1 = frame1 ? false : true;
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
  
    config->writeEntry("Tea",index);  

    bool ok;
    teatime = (*times.at(index)).toInt(&ok);
    if (!ok)
      teatime = 300;
  }
}


void TopLevel::config()
{
  QDialog *dlg = new QDialog(this, "", true);
  dlg->setCaption(i18n("Configure The Tea Cooker"));
  dlg->resize(320,180);

  QVBoxLayout *box = new QVBoxLayout(dlg,4,8);
  
  QGridLayout *grid = new QGridLayout(5,2);
  box->addLayout(grid);
  
  QSpinBox *spin = new QSpinBox(1,10000,10,dlg);
  spin->setFixedHeight(spin->sizeHint().height());

  QLabel *l = new QLabel(spin, i18n("Your Tea Time (s):"), dlg);
  l->setFixedSize(l->sizeHint());

  grid->addWidget(l,0,0);
  grid->addWidget(spin,0,1);

  QLineEdit *actionEdit = new QLineEdit(dlg);
  l = new QLabel(actionEdit, i18n("Action:"), dlg);
  l->setMinimumSize(l->sizeHint());
  actionEdit->setFixedHeight(actionEdit->sizeHint().height());

  grid->addWidget(l,1,0);
  grid->addWidget(actionEdit,1,1);

  QCheckBox *beep = new QCheckBox(i18n("Beep"), dlg);
  QCheckBox *popup = new QCheckBox(i18n("Popup"), dlg);
  beep->setFixedHeight(beep->sizeHint().height());
  popup->setFixedHeight(popup->sizeHint().height());

  grid->addWidget(beep, 2,1);
  grid->addWidget(popup, 3,1);

  grid->setRowStretch(4,1);

  QFrame *f = new QFrame(dlg);
  f->setFrameStyle(QFrame::HLine | QFrame::Raised);
  box->addStretch(1);
  box->addWidget(f);

  QHBoxLayout *hbox = new QHBoxLayout();
  box->addLayout(hbox);
  hbox->addStretch();

  QPushButton *ok = new QPushButton(i18n("OK"),dlg);
  ok->setDefault(true);
  QPushButton *cancel = new QPushButton(i18n("Cancel"), dlg);
  connect(ok, SIGNAL(pressed()), dlg, SLOT(accept()));
  connect(cancel, SIGNAL(pressed()), dlg, SLOT(reject()));
  ok->setFixedSize(ok->sizeHint());
  cancel->setFixedSize(cancel->sizeHint());

  hbox->addWidget(ok);
  hbox->addWidget(cancel);

  box->activate();

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
    times.remove(teas.last());
    QString n,n2;
    num = spin->value();
    n.sprintf(i18n("Other tea (%is)"), num);
    teas.append(n); n2.setNum(num); times.append(n2);
    menu->changeItem(n, teas.count()-1);

    KConfig *config = kapp->config();
    config->setGroup("Teas");
    config->writeEntry("Beep",beeping);
    config->writeEntry("Popup",popping);
    config->writeEntry("UserTea",num);
    config->writeEntry("Action",action);    
  }
}
